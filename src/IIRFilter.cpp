// Filter coefficients
void Event::filterCoeff()
{
  // If cutoff_1 == 0, use low pass
  // If cutoff_2 > nyquist, use high pass
  // If both okay, use bandpass
  float delta = ksacs[0]->GetSACHV(DELTA);
 	float a = cutoff_1*delta;
 	float b = cutoff_2*delta;
  if(a == 0) {					// Low pass
		c0 = b/(1+b);
 		c1 = c0;
 		d1 = (1-b)/(1+b);
    d2 = 0;
  }
  else if (b > 0.5) {		// High pass
  	c0 = 1/(1+a);
    c1 = -c0;
    d1 = (1-a)/(1+a);
    d2 = 0;
  }
  else {								// Bandpass
  	float opa = 1+a;
    float oma = 1-a;
    float opb = 1+b;
    float omb = 1-b;
  	c0 = -b/(opa*opb);
    c1 = 0;
    c2 = -c0;
    d1 = (opa*omb + oma*opb)/(opa*opb);
    d2 = -(oma*omb)/(opa*opb);
	}
}

void Event::filter()
{
	// If cutoff_1 is zero and cutoff_2
  // is larger than sampling frequency, don't do anything
  if(cutoff_1 == 0 && cutoff_2 > 0.5/ksacs[0]->GetSACHV(DELTA))
  	return;
  for(int k=0;k<n_ksac;k++) {
  	int npts = ksacs[k]->GetSACHV(NPTS);
  	if(npts > 0) {
  		float *data = ksacs[k]->GetData();
    	float last1 = data[0];
      float last2 = data[0];
      // Do point 0
			float x = c0*data[0];
      x += c1*last1;
      data[0] = x;
      // Do point 1
			x = c0*data[1];
      x += c1*last1 + c2*last2+d1*data[0];
      last1 = data[1];
    	data[1] = x;
      // And now the rest
  		for(int i=2;i<npts;i++) {
				float x = c0*data[i];
    		x += c1*last1 + c2*last2 + d1*data[i-1] + d2*data[i-2];
        last2 = last1;
      	last1 = data[i];
    		data[i] = x;
      }
  	}
  }
}

