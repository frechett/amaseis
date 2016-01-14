// JeffreyBullen class.  Creates an array of floating point numbers
// representing the Jeffrey Bullen travel time curves

class JB {
	public:
		JB(char *phase,float depth);
    ~JB();
    float *wave;
    int len;				// Size of array
};

