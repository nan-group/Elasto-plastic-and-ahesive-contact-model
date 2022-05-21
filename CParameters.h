#if !defined(CParameters_h)
#define CParameters_h

class CParameters
{
    public:
        CParameters();

		CParameters(double nsurface_energy);
									
		double kp;		// plastic stiffness 
		double ke;		// elastic stiffness 
		double kcp;		// plastic-cohesive stiffness 
		double kt;		// tangential stiffness 
		double f0;		// initial elastic force 
		double f02;		// initial plastic force 		

};



#endif