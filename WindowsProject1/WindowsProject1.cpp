#include <Windows.h>
#include <tchar.h>

// Function prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
void OpenSerialPort(HWND hwnd, const TCHAR* portName);
void CloseSerialPort();
void SendData(const TCHAR* data);
void ReadData();

// Global variables
HANDLE hSerial;

_Use_decl_annotations_ int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Step 1: Register the window class
    const TCHAR szClassName[] = _T("SerialPortApp");
    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szClassName;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("Window Registration Failed!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Create the window
    HWND hwnd = CreateWindow(szClassName, _T("Serial Port Interface"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 3: Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 4: Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Step 5: Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
    case WM_CREATE: {
        // Create buttons and other controls here
        CreateWindow(_T("BUTTON"), _T("Open Port"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30, hwnd, (HMENU)1, NULL, NULL);

        CreateWindow(_T("BUTTON"), _T("Close Port"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 10, 100, 30, hwnd, (HMENU)2, NULL, NULL);

        CreateWindow(_T("BUTTON"), _T("Send Data"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 50, 100, 30, hwnd, (HMENU)3, NULL, NULL);

        CreateWindow(_T("BUTTON"), _T("Read Data"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 50, 100, 30, hwnd, (HMENU)4, NULL, NULL);

        break;
    }

    case WM_COMMAND: {
        switch (static_cast<int>(LOWORD(w_param))) {
        case 1: // Open Port
            OpenSerialPort(hwnd, _T("COM1")); // Change COM1 to the desired port
            break;

        case 2: // Close Port
            CloseSerialPort();
            break;

        case 3: // Send Data
            SendData(_T("Hello, Serial Port!")); // Change the data as needed
            break;

        case 4: // Read Data
            ReadData();
            break;
        }
        break;
    }

    case WM_DESTROY: {
        // Cleanup and exit
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, message, w_param, l_param);
    }

    return 0;
}

void OpenSerialPort(HWND hwnd, const TCHAR* portName) {
    // Open the serial port
    hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
        MessageBox(hwnd, _T("Failed to open the serial port!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
    else 
        MessageBox(hwnd, _T("Serial port opened successfully!"), _T("Success"), MB_ICONINFORMATION | MB_OK);
}

void CloseSerialPort() {
    CloseHandle(hSerial);
}

void SendData(const TCHAR* data) {
    DWORD bytesWritten;

    // Send data to the serial port
    WriteFile(hSerial, data, static_cast<DWORD>(_tcslen(data) * sizeof(TCHAR)), &bytesWritten, NULL);
}

void ReadData() {
    TCHAR buffer[256];
    DWORD bytesRead;

    // Read data from the serial port
    if (ReadFile(hSerial, buffer, sizeof(buffer) - sizeof(TCHAR), &bytesRead, NULL)) {
        // Ensure null-termination within the buffer bounds
        if (bytesRead < sizeof(buffer) / sizeof(buffer[0])) 
            buffer[bytesRead] = _T('\0');
        else {
            // Handle the case where bytesRead exceeds the buffer size
            MessageBox(NULL, _T("Buffer overrun while reading data from the serial port!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
            return;
        }
        MessageBox(NULL, buffer, _T("Received Data"), MB_ICONINFORMATION | MB_OK);
    }
    else 
        MessageBox(NULL, _T("Failed to read data from the serial port!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
}
