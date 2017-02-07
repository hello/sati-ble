#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

struct hlo_ble_dev{
    int dev_id; /** device id from get_route (usually hci0) **/
    int dd;     /** the socket **/
};

static int advertise(const struct hlo_ble_dev *dev, int duration)
{
    int ret = hci_le_set_advertise_enable(dev->dd, 1, 1000);
    if (ret < 0) {
        printf("advertise start failed(%d) %d\n", errno, ret);
        return ret;
    }
    sleep(duration);
    ret = hci_le_set_advertise_enable(dev->dd, 0, 1000);
    if (ret < 0) {
        printf("advertise stop failed %d\n", ret);
        return ret;
    }
    return 0;
}
static int set_device_name(const struct hlo_ble_dev *dev, const char *name)
{
    int ret = hci_write_local_name(dev->dd, name, 1000);
    if (ret < 0) {
        return  -1;
    }
    char check_name[128] = {0};
    hci_read_local_name(dev->dd, sizeof(check_name), check_name, 1000);
    printf("set device name to: %s\n", check_name);
    return 0;
}
static int init_dev(struct hlo_ble_dev *dev)
{
    dev->dev_id = hci_get_route(NULL);
    dev->dd = hci_open_dev( dev->dev_id );
    if (dev->dev_id < 0 || dev->dd < 0) {
        goto init_fail;
    }
    return 0;
init_fail:
    return -1;
}
int main(int argc, char *argv[])
{
    struct hlo_ble_dev dev;
    if (0 != init_dev(&dev)){
        printf("unable to initialize ble device!\n");
        return -1;
    }
    set_device_name(&dev, "Sati");
    advertise(&dev, 20);
    return 0;
}
