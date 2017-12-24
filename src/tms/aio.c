/*
 *
 */

#include "aio.h"

static void end_io(aio_device_t *dev, aio_status_t staus)
{
        tms_delete_timer(dev->timer);
        dev->info->status = staus;
        dev->unlock(dev);
        dev->aio_callback(dev);
}

static void timer_callback(timer_param_t param)
{
        aio_device_t *dev = (aio_device_t*) param;
        dev->chancel(dev);
        end_io(dev, AIO_STATUS_TIMEOUT);
}

static void begin_io(aio_device_t *dev, aio_info_t *info,
                void (*aio_callback)(struct _aio_device *dev))
{
        dev->info = info;
        dev->aio_callback = aio_callback;
        dev->count = 0;
        dev->actual = info->pchar;
        dev->timer = tms_create_timer(&timer_callback, dev);
        tms_start_timer(dev->timer, 10);
}

static void read_callback(struct _aio_device *dev, char data)
{
        *dev->actual++ = data;
        if (++dev->count == dev->info->size) {
                end_io(dev, AIO_STATUS_OK);
        } else {
                tms_start_timer(dev->timer, 10);
                dev->read(dev, &read_callback);
        }
}

static void write_callback(struct _aio_device *dev)
{
        if (++dev->count == dev->info->size) {
                end_io(dev, AIO_STATUS_OK);
        } else {
                tms_start_timer(dev->timer, 10);
                dev->write(dev, *dev->actual++, &write_callback);
        }
}

static void lock_callback(struct _aio_device *dev)
{
        if (dev->info->status == AIO_STATUS_PREPARE_READ) {
                dev->info->status = AIO_STATUS_READ;
                tms_start_timer(dev->timer, 10);
                dev->read(dev, &read_callback);
        } else if (dev->info->status == AIO_STATUS_PREPARE_WRITE) {
                dev->info->status = AIO_STATUS_WRITE;
                tms_start_timer(dev->timer, 10);
                dev->write(dev, *dev->actual, &write_callback);
        } else {
                end_io(dev, AIO_STATUS_ERROR);
        }
}

extern void aio_set_timeuot(aio_device_t *dev, timer_counter_t timeout){
        dev->timeout=timeout;
}

extern timer_counter_t aio_get_timeuot(aio_device_t *dev){
        return dev->timeout;
}

extern aio_return_t aio_read(aio_device_t *dev, aio_info_t *info,
                void (*aio_callback)(struct _aio_device *dev))
{
        begin_io(dev, info, aio_callback);
        dev->info->status = AIO_STATUS_PREPARE_READ;
        return dev->lock(dev, &lock_callback);
}

extern aio_return_t aio_write(aio_device_t *dev, aio_info_t *info,
                void (*aio_callback)(struct _aio_device *dev))
{
        begin_io(dev, info, aio_callback);
        dev->info->status = AIO_STATUS_PREPARE_WRITE;
        return dev->lock(dev, &lock_callback);
}

extern aio_return_t aio_clean(aio_device_t *dev)
{
        return dev->clean(dev);
}

