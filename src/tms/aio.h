/*
 * Объявления для организации асинхронного ввода-вывода.
 */

#ifndef AIO_H_
#define AIO_H_

#include "tms.h"

#ifdef __cplusplus
extern "C" {
#endif

        typedef unsigned char aio_size_t;

        typedef enum {
                AIO_STATUS_OK = 0,
                AIO_STATUS_FREE,
                AIO_STATUS_PREPARE_READ,
                AIO_STATUS_PREPARE_WRITE,
                AIO_STATUS_READ,
                AIO_STATUS_WRITE,
                AIO_STATUS_TIMEOUT,
                AIO_STATUS_ERROR
        } aio_status_t;

        typedef struct _aio_info {
                aio_size_t size;
                union {
                        void *pvoid;
                        char *pchar;
                        int *pint;
                };
                volatile aio_status_t status;
        } aio_info_t;

        typedef enum {
                AIO_RETURN_OK = 0,
                AIO_RETURN_ERROR,
                AIO_RETURN_NOT_IMPLEMENT,
                AIO_RETURN_BUSY
        } aio_return_t;

        typedef struct _aio_device {
                aio_return_t (*lock)(struct _aio_device *dev,
                                void (*lock_callback)(struct _aio_device *dev));
                aio_return_t (*unlock)(struct _aio_device *dev);
                /*
                 *
                 */
                aio_return_t (*clean)(struct _aio_device *dev);
                /*
                 * Отмена текущей операции обмена. Это может быть вызвано по завершению ожидания
                 * ответа от устройства.
                 */
                aio_return_t (*chancel)(struct _aio_device *dev);
                aio_return_t (*read)(struct _aio_device *dev,
                                void (*read_callback)(struct _aio_device *dev,
                                                char data));
                aio_return_t (*write)(struct _aio_device *dev, char data,
                                void (*write_callback)(
                                                struct _aio_device *dev));
                aio_return_t (*flush)(struct _aio_device *dev,
                                void (*flush_callback)(
                                                struct _aio_device *dev));
                aio_info_t *info;
                /*
                 * Функция обратного вызова, которая передается в качестве последнего параметра
                 * функций aio_read, aio_write и aio_flush.
                 * Эта функция вызывается после завершения обмена информацией.
                 */
                void (*aio_callback)(struct _aio_device *dev);
                /*
                 * Счетчик обработанных символов.
                 */
                aio_size_t count;
                char *actual;
                timer_id_t timer;
                timer_counter_t timeout;

        } aio_device_t;

        extern void aio_set_timeuot(aio_device_t *dev, timer_counter_t timeout);
        extern aio_return_t aio_read(aio_device_t *dev, aio_info_t *info,
                        void (*aio_callback)(struct _aio_device *dev));
        extern aio_return_t aio_write(aio_device_t *dev, aio_info_t *info,
                        void (*aio_callback)(struct _aio_device *dev));
        extern aio_return_t aio_clean(aio_device_t *dev);

#define AIO_DEVICE_IIC iic

/*
 *Макрос для вызова функции инициализации устройсва.
 */
#define aio_init(name, aio_device) aio_init_&&name(aio_device)

#ifdef __cplusplus
}
#endif

#endif /* AIO_H_ */
