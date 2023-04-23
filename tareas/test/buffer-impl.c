/* Implemente aqui el driver para /dev/buffer */

/* Necessary includes for device drivers */
#include <linux/init.h>
/* #include <linux/config.h> */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_from/to_user */

#include "kmutex.h"

MODULE_LICENSE("Dual BSD/GPL");

static int buffer_open(struct inode *inode, struct file *filp);
static int buffer_release(struct inode *inode, struct file *filp);
static ssize_t buffer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t buffer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

void buffer_exit(void);
int buffer_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations buffer_fops = {
  read: buffer_read,
  write: buffer_write,
  open: buffer_open,
  release: buffer_release
};

/* Declaration of the init and exit functions */
module_init(buffer_init);
module_exit(buffer_exit);

/*** El driver para lecturas sincronas *************************************/

#define TRUE 1
#define FALSE 0

/* Global variables of the driver */

int buffer_major = 61;     /* Major number */

/* Buffer to store data */
#define MAX_SIZE 80

static char (*buffer_buffer)[3];

static int i_insert;
static int i_extract;
static int size[3];
//static int in, out, size;

/* El mutex y la condicion para pipe */
static KMutex mutex;
static KCondition cond;

int buffer_init(void){
    int rc;
    rc = register_chrdev(buffer_major, "buffer", &buffer_fops);
    if (rc < 0) {
        printk("<1>buffer: cannot obtain major number\n");
        return rc;
    }
    printk("registro exitoso\n");

    i_insert = 0;
    i_extract = 0;
    size[0] = 0; 
    size[1] = 0;
    size[2] = 0;
    m_init(&mutex);
    c_init(&cond);
    printk("inicializacion de variables globales exitosa\n");

    buffer_buffer = kmalloc(sizeof(char[3][MAX_SIZE]), GFP_KERNEL);

    if(buffer_buffer == NULL){
        printk("kmalloc no exitoso\n");
        buffer_exit();
        return -ENOMEM;
    }
    printk("kmalloc exitoso\n");
    
    printk("<1>Inserting buffer module\n");
    return 0;
}

void buffer_exit(void){
    unregister_chrdev(buffer_major, "buffer");

    if(buffer_buffer != NULL){
        kfree(buffer_buffer);
    }
    printk("<1>Removing buffer module\n");
}

static int buffer_open(struct inode *inode, struct file *filp){
    //char *mode=   filp->f_mode & FMODE_WRITE ? "write" :
    //                filp->f_mode & FMODE_READ ? "read" :
    //                "unknown";
    //printk("<1>open\n");
    return 0;
}

static int buffer_release(struct inode *inode, struct file *filp){
    //printk("<1>release\n");
    return 0;
}

static ssize_t buffer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
    ssize_t rc = 0;
    //printk("<1>read\n");
    //m_lock(&mutex);
    /*
    if(*f_pos > 0){
        rc = 0;
        goto epilog;
    }

    while (size[i_extract] == 0) {
         si el buffer esta vacio, se espera 
        if (c_wait(&cond, &mutex)) {
            printk("<1>read interrupted\n");
            rc= -EINTR;
            goto epilog;
        }
    }

    if (count > size[i_extract]) {
        count= size[i_extract];
    }

     Transfiriendo datos hacia el espacio del usuario 
    if (copy_to_user(buf, buffer_buffer[i_extract]+*f_pos, count)!=0) {
         el valor de buf es una direccion invalida 
        rc= -EFAULT;
        goto epilog;
    }

    printk("<1>reading bytes\n");

    *f_pos += count;
    rc = count;
    i_extract++;
    */
    //epilog:
        //c_broadcast(&cond);
        //m_unlock(&mutex);
    return rc;
}

static ssize_t buffer_write( struct file *filp, const char *buf, size_t count, loff_t *f_pos){
    //printk("<1>write\n");
    
    //m_lock(&mutex);
    /*
    while (size[i_insert]==MAX_SIZE) {
       si el buffer esta lleno, el escritor espera 
      if (c_wait(&cond, &mutex)) {
        printk("<1>write interrupted\n");
        count= -EINTR;
        goto epilog;
        }
    }

    if (copy_from_user(buffer_buffer, buf, count)!=0) {
       el valor de buf es una direccion invalida 
      count= -EFAULT;
      goto epilog;
    }
    printk("<1>writing bytes\n");
    c_broadcast(&cond);
    */
    //epilog:
        //m_unlock(&mutex);
    return count;
}