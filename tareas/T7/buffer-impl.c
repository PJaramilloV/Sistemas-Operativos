/* Implemente aqui el driver para /dev/buffer */
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

#include "kmutex.h" /* mutex and cond */

MODULE_LICENSE("Dual BSD/GPL"); /* license */


// pre-declare operations
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


module_init(buffer_init);
module_exit(buffer_exit);


/* Global variables */
int buffer_major = 61; // major

#define TRUE 1
#define FALSE 0
#define ROWS 3
#define COLS 80

static char (*buffer_buffer)[COLS];
static int reading, writing, in_pos, out_pos, data_here, full;
static size_t bytes[ROWS] = {0,0,0};

static KMutex mutex;
static KCondition cond;

// Must register major number, initiate variables and allocate memory 
int buffer_init(void){
	int rc;
	rc = register_chrdev(buffer_major, "buffer", &buffer_fops);
	if(rc < 0){
		printk("[buffer]: cannot obtain major number %d\n", buffer_major);
		return rc;
	}

	// init without reading or writing and all values 0
	reading = writing = in_pos = out_pos = data_here = full = 0;
	// Mutex and Condition
	m_init(&mutex);
	c_init(&cond);
	// Buffer space
	buffer_buffer = kmalloc(sizeof(char[ROWS][COLS]), GFP_KERNEL); // make 2D buffer
	if(buffer_buffer == NULL){
		buffer_exit();
		printk("[buffer]: failed to allocate buffer memory\n");
		return -ENOMEM;
	}

	printk("[buffer]: successful buffer module insertion\n");
	return 0;
}

// Must free major number and allocated memory
void buffer_exit(void){
	// Free major number
	unregister_chrdev(buffer_major, "buffer");

	// free buffer
	if(buffer_buffer != NULL){
		kfree(buffer_buffer);
	}

	printk("[buffer]: buffer module removed\n");
}



static int buffer_open(struct inode *inode, struct file *filp){
	char *mode=   filp->f_mode & FMODE_WRITE ? "write" :
                filp->f_mode & FMODE_READ ? "read" :
                "unknown";
	m_lock(&mutex);
	if(filp->f_mode & FMODE_WRITE){
		// Wait to enter when nobody else is using
		while(writing){
			if(c_wait(&cond, &mutex)){
				printk("[buffer]: writer waiting to enter interrupted\n");
				c_broadcast(&cond);
				m_unlock(&mutex);
				return -EINTR;
			}
		}
		writing = TRUE;
	} else if(filp->f_mode & FMODE_READ){
		reading++;
	}
	c_broadcast(&cond);
	m_unlock(&mutex);
  printk("[buffer]: open %p for %s\n", filp, mode);
  return 0;
}

static int buffer_release(struct inode *inode, struct file *filp){
	char *mode=   filp->f_mode & FMODE_WRITE ? "write" :
                filp->f_mode & FMODE_READ ? "read" :
                "unknown";
	m_lock(&mutex);
	if(filp->f_mode & FMODE_WRITE){
		writing = FALSE;
		c_broadcast(&cond);
	} else if(filp->f_mode & FMODE_READ){
		reading--;
		if(reading == 0){
			c_broadcast(&cond);
		}
	}
	printk("[buffer]: release (%p) from mode %s\n", filp , mode);
	m_unlock(&mutex);
	return 0;
}

static ssize_t buffer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
  	ssize_t rc;	
	m_lock(&mutex);
	printk("[buffer]: read start\n");
	// While the buffer is empty, wait
	while(!data_here){
		if(c_wait(&cond, &mutex)){
			printk("[buffer]: read interrupted\n");
      		rc= -EINTR;
			m_unlock(&mutex);
			return rc;
		}
	}

	// Make sure to read only what is present on the buffer
	if(count > bytes[out_pos]){
		count = bytes[out_pos];
		// if there is nothing to read
		if(count == 0){
			// move reading pointer and return
			out_pos++;
			out_pos %= ROWS;
			rc = count;
			if(out_pos == in_pos){
				// Buffer is now empty, future readers must wait
				data_here = FALSE;
			}
			printk("[buffer]: read close\n");
			m_unlock(&mutex);
			return rc;
		}
		// read underflow error
		if(count < 0){
			printk("[buffer]: read underflow\n");
			rc = -EILSEQ;
			m_unlock(&mutex);
			return rc;
		}
	}
	printk("[buffer]: read %d bytes at row %d\n", (int)count, out_pos);
	
	/* Transfering data to user space */
	if(copy_to_user(buf, buffer_buffer[out_pos], count)!=0){
		// If the transfer fails, return error code
		rc= -EFAULT;
		m_unlock(&mutex);
		return rc;
	}
	

	// mark off the read bytes
	bytes[out_pos] -= count;
	full = FALSE;
	rc = count;
	m_unlock(&mutex);
	printk("[buffer]: read success\n");
	return rc;
}

static ssize_t buffer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
	ssize_t rc;	
	m_lock(&mutex);

	// While the buffer is full, wait
	while(full){
		if(c_wait(&cond, &mutex)){
			printk("[buffer]: write interrupted\n");
      		rc= -EINTR;
			m_unlock(&mutex);
			return rc;
		}
	}
	
	// Make sure not to overflow the buffer
	if(count > COLS){
		count = COLS;
	}

	printk("[buffer]: write %d bytes at row %d\n", (int)count, in_pos);

	/* Transfering data from user space */
	if (copy_from_user(buffer_buffer[in_pos], buf, count)!=0) {
    	/* el valor de buf es una direccion invalida */
    	rc= -EFAULT;
    	m_unlock(&mutex);
		return rc;
  	}

	// Successful data write, update details
	bytes[in_pos] = count;
	in_pos++;
	in_pos %= ROWS;
	data_here = TRUE;
	if(in_pos == out_pos){
		// Buffer is now full
		full = TRUE;
	}
	rc = count;	

	m_unlock(&mutex);
	return rc;
}