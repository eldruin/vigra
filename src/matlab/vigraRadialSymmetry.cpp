/*++++++++++++++++++++INCLUDES+and+Definitions++++++++++++++++++++++++*/

#include <vigra/matlab.hxx>
#include <string>


//this could be a typedef but if you want outType to be the same type as inType then you can just 
//set outType to T
#define outType double
#define vigraFunc vigraRadialSymmetry
/*++++++++++++++++++++++++++HELPERFUNC+++++++++++++++++++++++++++++++*/
/* This is used for better readibility of the test cases            .
/* Nothing to be done here.
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
using namespace vigra;

int cantorPair(int x, int y){
		return (int)(((x+y)*(x+y+1))/2+y);
}
int cantorPair(int x, int y, int z){
		return cantorPair(cantorPair(x,y),z);
}

template <int x, int y>
struct cP{
	enum { value = (int)(((x+y)*(x+y+1))/2+y)};
};

template <int x, int y, int z>
struct cP3{
	enum { value = cP<cP<x, y>::value, z>::value};
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* The Optons struct contains all the necassary working data and 
/* options for the vigraFunc. This is the minimal struct
/* Add fields as necessary
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
template <class T>
struct options{
	int numOfDim;
	double scale;
	
	BasicImageView<T>  in;
	MultiArrayView<3,T> in3D;

	BasicImageView<YOURTYPE>  out;
	MultiArrayView<3,YOURTYPE> out3D;
	
	options(int nofDim, double scle){
		numOfDim = nofDim
		scale = scle;
	}
};

// Quick and dirty macro for filling numerical options fields.
#define fillOptNumericField(name); \
		mxArray* name =mxGetField(inputs[1], 0, #name);\
		opt.name = (name!=NULL&&mxIsNumeric(name))?\
							mxGetScalar(name) : opt.name;
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* This function does all the work
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
template <class T ...>
void vigraFunc(matlab::OutputArray outputs, matlab::InputArray inputs){
	// Constant definition for readibility
	enum {IMAG = 2, VOLUME = 3}dim;
	
	//Default Options
	options<T> opt(mxGetNumberOfDimensions(inputs[0]), 1.0);
	
	{//Preconditions on default options
	if(opt.numOfDim > IMAG)  
						mexErrMsgTxt("Currently InputArray may only have 2 dimensions");
	if(inputs.isEmpty(0)) 
						mexErrMsgTxt("Input Image is empty!");
					
	}	
	
	//Map data to option fields
	if(opt.numOfDim == IMAG){
		opt.in = matlab::getImage<T>(inputs[0]);
		opt.out = matlab::createImage<double>(opt.in.width(), opt.in.height(), outputs[0]);
		opt.in3D = matlab::getMultiArray<3, T>(inputs[0]);
		//Lets opt3D View the same data as out.
		opt.out3D = MultiArrayView<3, outType>(opt.in3D.shape(), (outType*)opt.out.data());
	}else{
		opt.in3D = matlab::getMultiArray<3, T>(inputs[0]);
		opt.out3D = matlab::createMultiArray<3,double>(opt.in3D.shape(), outputs[0]);
	}
	
	//User supplied Options
	if(inputs.isValid(1)){	
		filloptNumericField(scale);
		if(scale < 0){
			mexWarnMsgTxt("Negative scale parameter. Using default: 1.0");
			opt.scale = 1.0;
		}
	}

	

	radialSymmetryTransform(srcImageRange(opt.in), destImage(opt.out), opt.scale);
	
}



/*+++++++++++++++++++++++MexEntryFunc++++++++++++++++++++++++++++++++*/
/* DELETE LINES IF A CERTAIN CLASS IS NOT SUPPORTED
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void vigraMexFunction(matlab::OutputArray outputs, matlab::InputArray inputs)
{    
	mxClassID inClass = mxGetClassID(inputs[0]);
	switch(inClass){
		case mxDOUBLE_CLASS:
			vigraFunc<double>(outputs, inputs);	break;
		case mxSINGLE_CLASS:
			vigraFunc<float>(outputs, inputs);		break;
        case mxINT8_CLASS:
			vigraFunc<Int8>(outputs, inputs);		break;
		case mxINT16_CLASS:
			vigraFunc<Int16>(outputs, inputs);		break;
		case mxINT32_CLASS:
			vigraFunc<Int32>(outputs, inputs);		break;
		case mxINT64_CLASS:
			vigraFunc<Int64>(outputs, inputs);		break;
        case mxUINT8_CLASS:
			vigraFunc<UInt8>(outputs, inputs);		break;
		case mxUINT16_CLASS:
			vigraFunc<UInt16>(outputs, inputs);	break;
		case mxUINT32_CLASS:
			vigraFunc<UInt32>(outputs, inputs);	break;
		case mxUINT64_CLASS:
			vigraFunc<UInt64>(outputs, inputs);	break;		
		default:
			mexErrMsgTxt("Input image must have type 'uint8'-16-32-64', 'int8-16-32-64' 'single' or 'double'.");
	}
}
