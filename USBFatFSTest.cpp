#include "daisy_patch_sm.h"
#include "daisy_patchsm_usb.h"
#include "fatfs.h"
#include "usbh_msc.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM_USB   hw;
FIL            file; /**< Can't be made on the stack (DTCMRAM) */
FatFSInterface fsi;
FRESULT fileResult;

#define IO_BUFFER_SIZE 100
#define TEST_FILE_NAME "USB_Test.txt"
char testFilePath[IO_BUFFER_SIZE];

bool isUsbConnected = false;
bool isUsbConfigured = false;
bool wasConfigLoadAttempted = false;
bool isConfigChanged = false;

Switch  button;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    hw.ProcessAllControls();
    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
    }
}

/* USB Host callbacks */
void USBConnectCallback(void* userdata) {
    isUsbConnected = true;
    hw.PrintLine("USB Connected");
    hw.usbHost.RegisterClass(USBH_MSC_CLASS); // The class must be registered! 
}
void USBDisconnectCallback(void* userdata) {
    isUsbConnected = false;
    isUsbConfigured = false;
    hw.PrintLine("USB Disconnected");
    hw.usbHost.GetReady();
}
void USBClassActiveCallback(void* userdata) {
    hw.PrintLine("USB class active");
    hw.usbHost.GetReady();
    isUsbConfigured = true;
}
void USBErrorCallback(void* userdata) {
    hw.PrintLine("USB Error");
}


/* This is our read/write test, triggered now by a button press*/
void writeTest() {

    hw.usbHost.Process();

    FRESULT fres = FR_DENIED; 

    /** Strings for write/read to text file */
    const char *test_string = "Testing Daisy Patch SM";
    const char* usbPath = hw.fatfs_interface.GetUSBPath();
    snprintf(testFilePath, IO_BUFFER_SIZE, "%s%s", usbPath, TEST_FILE_NAME);

    /** Give some feedback on whether the drive is ready before we attempt */
    int ready = hw.usbHost.GetReady();
    hw.PrintLine("Application state: %d",ready);
    
    /** Write Test */
    fres = f_open(&file, testFilePath, (FA_CREATE_ALWAYS | FA_WRITE));
    if(fres == FR_OK)
    {
        UINT   bw  = 0;
        size_t len = strlen(test_string);
        fres       = f_write(&file, test_string, len, &bw);
        hw.PrintLine("WRITE OK!");
    } else {
        hw.PrintLine("Write test f_open failed with code: %d",fres);
    }
    f_close(&file);
    if(fres == FR_OK)
    {
        /** Read Test only if Write passed */
        fres = f_open(&file, testFilePath, (FA_OPEN_EXISTING | FA_READ));
        if(fres == FR_OK)
        {
            UINT   br = 0;
            char   readbuff[32];
            size_t len = strlen(test_string);
            fres       = f_read(&file, readbuff, len, &br);
            hw.PrintLine("READ OK!");
        } else {
            hw.PrintLine("Read test f_open failed with code: %d",fres);
        }
        f_close(&file);
    }

    bool usb_pass = fres == FR_OK;
    hw.PrintLine("USB Test\t-\t%s", usb_pass ? "PASS" : "FAIL");
}

int main(void)
{
    hw.Init();
    hw.StartLog(true);
    hw.PrintLine("Daisy Patch SM started. Test Beginning");

    /* We'll trigger the test with a button connected to B7 */
    button.Init(DaisyPatchSM::B7, hw.AudioCallbackRate());

    /* SET UP USB*/
    hw.PrepareMedia(USBConnectCallback, USBDisconnectCallback, USBClassActiveCallback, USBErrorCallback);

    uint32_t now;
    now = System::GetNow();

    hw.usbHost.Process();
    bool ready = hw.usbHost.GetReady();
    hw.PrintLine("Is ready: %d",ready);

    while(1)
    {
        now = System::GetNow();
        hw.SetLed((now & 2047) > 60);

        hw.usbHost.Process();
        button.Debounce();

        if (button.RisingEdge()) {
            writeTest();
            System::Delay(100);
        }
        
    }
}
