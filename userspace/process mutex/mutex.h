/*
 * mutex.h
 *
 *  Created on: 2015Äê4ÔÂ12ÈÕ
 *      Author: Administrator
 */

#ifndef TEAPOTSET_USERSPACE_PROCESS_MUTEX_MUTEX_H_
#define TEAPOTSET_USERSPACE_PROCESS_MUTEX_MUTEX_H_
#include <semaphore.h>

sem_t * mutex_init(char *mutex_sem);
int mutex_lock(sem_t *mutex);
int mutex_unlock(sem_t *mutex);
int mutex_close(sem_t *mutex);


int mutex_fcntl_init(char *mutex_filename);
int mutex_fcntl_lock_r(int fd);
int mutex_fcntl_lock_w(int fd);
int mutex_fcntl_unlock(int fd);
int mutex_fcntl_close(int fd);

int mutex_flock_init(char *mutex_filename);
int mutex_flock_lock(int fd);
int mutex_flock_unlock(int fd);
int mutex_flock_close(int fd);

#endif /* TEAPOTSET_USERSPACE_PROCESS_MUTEX_MUTEX_H_ */
