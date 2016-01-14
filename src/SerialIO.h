#if !defined(serialio_h)              // Sentry, use file only if it's not already included.
#define serialio_h

/********************
Serial IO class
*******************/

class SerialIO {
	public:
		SerialIO(int buf_size);
    ~SerialIO();
    bool cominit(void);
		unsigned char * chars_avail(	int &size,  bool parity);
    void initDATAQ();
		void com_port_off(void);
    DWORD Send(char *,int n);
    bool opened;
  private:
  	HANDLE comhandle;			// Handle for communications
    int buff_size;
    unsigned char * buffer;
};
#endif
