// Auxiliar sobre Drivers
#include <linux/init.h>

#include "kmutex.h"

MODULE_LICENSE("Dual BSD/GLP");

/* syncread functions */
int syncread_open(struct inode *inode, struct file *filp);
int syncread_release(struct inode *inode, struct file *filp);
ssize_t syncread_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t syncread_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void syncread_exit(void);
int syncread_init(void);

/* Structure that declases the file */

/* access functions*/
struct file_operations syncread_fops = {
  read: syncread_read,
  write: syncread_write,
  open: syncread_open,
  release: syncread_release
};

/* Declaration of the init and exit functions */
module_init(syncread_init);
module_init(syncread_exit);

/* Driver para lecturas sincronas */
#define TRUE 1
#define FALSE 0

/* Globar vars of driver */
#define MAX_SIZE 8192
int syncread_major = 61;
static char *syncread_buffer;  // buffer de contenido
static ssize_t curr_size;

// Exclusion mutua
static int wrinting;            // escritor escribiendo?
static int readers;             // N° de lectores
static int pend_open_write;     // N° de escritores esperando

static KMutex mutex;
static KCondition cond;


int syncread_init(void){
  int rc;
  rc = register_chrdev(syncread_major, "syncread", &syncread_fops)
  if(rc < 0){
    printk("syncread_init error")
    return rc;
  }
  writing = FALSE;
  readers = 0;
  pend_open_write = 0;
  curr_size = 0;
  m_init(&mutex);
  c_init(&cond);
  syncread_buffer = kmalloc(MAX_SIZE, GFP_KERNEL); // how much memory, from where

  if(syncread_read == NULL){
    syncread_exit();
    return -ENOMEM;
  }
  reutrn 0;
}

void syncread_exit(void){
  unregister_chrdev(syncread_major, "syncread");
  if( syncread_buffer != NULL ){
    kfree(syncread_buffer);
  }
}

int syncread_open(struct inode *inode, struct file *filp){
  m_lock(&mutex)
  if (filp->f_mode & FMODE_WRITE){
    pend_open_write++;
    while(wrinting){
      if(c_wait(&cond, &mutex)){  // Se despierta la cond por una interrupcion inesperada
        pend_open_write--;
        rc = -EINTR;
        m_unlock(&mutex);
        return rc;
      }
    }
    wrinting = TRUE;
    pend_open_write--;
    curr_size = 0;
  }else if(filp->f_mode & FMODE_READ){
    readers++;
  }
  m_unlock(&mutex);
  return rc;
}

int syncread_release(struct inode *inode, struct file *filp){
  m_lock(&mutex);
  if(filp->f_mode & FMODE_WRITE){
    wrinting = FALSE;
    c_broadcast(&cond);
  }else if(filp->f_mode & FMODE_READ){
    readers--;
    if(readers == 0){
      c_broadcast(&cond);
    }
  }
  m_unlock(&mutex);
  return 0;
}

ssize_t syncread_read(struct file *filp, char *buf, 
                        size_t count, loff_t *f_pos){
  ssize_t rc;
  m_lock(&mutex);
  while( curr_size <= *f_pos && wrinting){
    c_wait(&cond, &mutex);
  }
  if(count > curr_size - *f_pos){
    count = curr_size;
  }
  if(copy_to_user(buf, syncread_buffer + *f_pos, count) != 0){
    /* el valor de buf es una direccion invalida */
    rc= -EFAULT;
    goto epilog;    
  }
  *f_pos += count;
  rc = count;

epilog:
  m_unlock(&mutex);
  return rc;
}

ssize_t syncread_write(struct file *filp, const char *buf, 
                        size_t count, loff_t *f_pos){
  ssize_t rc;
  loff_t last;

  m_lock(&mutex);

  last= *f_pos + count;
  if (last>MAX_SIZE) {
    count -= last-MAX_SIZE;
  }
  printk("<1>write %d bytes at %d\n", (int)count, (int)*f_pos);

  /* Transfiriendo datos desde el espacio del usuario */
  if (copy_from_user(syncread_buffer+*f_pos, buf, count)!=0) {
    /* el valor de buf es una direccion invalida */
    rc= -EFAULT;
    goto epilog;
  }

  *f_pos += count;
  curr_size= *f_pos;
  rc= count;
  c_broadcast(&cond);

epilog:
  m_unlock(&mutex);
  return rc;
}