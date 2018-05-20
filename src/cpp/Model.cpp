

#include "../include/Model.h"

// Constructor
main_ns::model_ns::model_cls::model_cls(const main_ns::address_ns::address_cls *ModelInput):
input (ModelInput){

std::cout << " Reading model ..." << std::endl;
std::cout << std::endl;

NNBndry = 1;      // Number of nodes on the bounday of the DRM
NNLayer = 2;      // Number of nodes on the bounday layer of the DRM


// Input File
std::cout << " Open the input file ..." << std::endl;
std::cout << std::endl;
InputFile.open (input->Input_Dir.c_str(), std::ios::in );

}


/*
###################################################################################################

Purpose: This function reads the basic data for the model, so that we can form the arrays.

Developed by: Babak Poursartip
 
The Institute for Computational Engineering and Sciences (ICES)
The University of Texas at Austin	
================================= V E R S I O N ===================================================
V0.00: 05/14/2018 - Subroutine initiated.
V0.01: 05/15/2018 - Initiated: Compiled without error for the first time.

###################################################################################################
*/

void main_ns::model_ns::model_cls::InputBasic()
{


double Total_Time; // Total simulation time

std::string TempS;      // Temporary variable for reading strings from input files

// = Code =========================================================================================

std::cout << "Input Basic ..." << std::endl;

getline(InputFile,TempS);
getline(InputFile,TempS);
getline(InputFile,TempS);
getline(InputFile,TempS);

InputFile >> NMat >> NPM >> NNodePWaveL >> LoadType >> Dis_History >> Solver;

NInt =3;
NDim =1;
NDOF =1;
NNode=3;

getline(InputFile,TempS);
getline(InputFile,TempS);
getline(InputFile,TempS);


InputFile >> Beta >> Gama >> DT >> Total_Time;
NStep = (int)(Total_Time / DT);

getline(InputFile,TempS);
getline(InputFile,TempS);
getline(InputFile,TempS);

InputFile >> L >> Alpha >> P >> A >> OShFunc;

getline(InputFile,TempS);
getline(InputFile,TempS);
getline(InputFile,TempS);
InputFile >> Wave_Type >> Wave_Func >> alpha1 >> alpha2 >> amplitude >> omega >> NEl_DRM;

}

/*
###################################################################################################

Purpose: This function reads the data in the forms of arrays for the model.

Developed by: Babak Poursartip
 
The Institute for Computational Engineering and Sciences (ICES)
The University of Texas at Austin
================================= V E R S I O N ===================================================
V0.00: 05/14/2018 - Subroutine initiated.
V0.01: 05/15/2018 - Initiated: Compiled without error for the first time.

###################################################################################################
*/
void main_ns::model_ns::model_cls::InputArrays()
{


int imat, i;            // loop variable
int NEl_Layer;          // Number of elements in each layer


double Layer;           // Layer size
double m1, m2;          // material properties
double Layer_check;     // Layer size
double G;               // Module (Shear or Elastic)
double rho;             // Density
double c;               // Wave velocity
double wavelength;      // wavelength of the wave
double h;               // element size

std::string TempS;   // Temporary variable for reading strings from input files


  
// = Code =========================================================================================


// Allocating required 1D arrays - vectors
std::cout << " Allocating arrays ..." << std::endl;
Length      = new double[NMat];  // Material Type of Elements
NoBndry_DRM = new int[NNBndry];  // vector that holds the node numbers on the DRM boundary- There is only one node on the DRM boundary
NoLayer_DRM = new int[NNLayer];  // vector that holds the node numbers on the DRM layer. Ther are only two nodes in the layer in a 1D wave motion.

Nodal_History = new int[Dis_History];
Loc_History = new double[Dis_History];

PMat = new double *[NMat];  // Properties of Materials
  for (int i = 0; i<NMat; i++){
    PMat[i] = new double[NPM];
  }

Element_Layer = new int[NMat];
Layer_Depth   = new int[NMat];


std::cout << "Reading input files (arrays) ..." << std::endl;

getline(InputFile, TempS);
getline(InputFile, TempS);

std::cout << "Material properties ..." << std::endl;
  for (i = 0; i<NMat; i++) {
    InputFile >> imat >> m1 >> m2;
    PMat[imat - 1][0] = m1;
    PMat[imat - 1][1] = m2;
  }

std::cout << "Layeres ..." << std::endl;
getline(InputFile, TempS);
getline(InputFile, TempS);
Layer_check = 0.0;
  for (i = 0; i<NMat; i++) {
    InputFile >> Layer;
    Layer_check += Layer;
    Layer_Depth[i] = Layer;
    Length[i] = -L + Layer_check;
  }

  if (Layer_check != L) {  // To see if the length of layers match up with the total length
    std::cout << "Warning: Total sum of layers is not equal to the total length:  " 
              << L << " /= " << Layer_check << std::endl;
    return;
  }

// Reading the location of nodes for recording the history of displacement
std::cout << "Time history locations ..." << std::endl;
getline(InputFile, TempS);
getline(InputFile, TempS);
  for (i = 0; i<Dis_History; i++) {
    InputFile >> Loc_History[i];
  }

// compute discretization information
NEl = 0;
  for (i = 0; i < NMat; i++) {
    G   = PMat[i][0];
    rho = PMat[i][1];
    c = sqrt(G / rho);
    wavelength = c / omega;
    h = 2 * wavelength / NNodePWaveL;  // max allowable element size
    NEl_Layer = (int)(ceil(Layer_Depth[i]/h));  // NEl_Layer
    Element_Layer[i] = NEl_Layer;
    NEl += Element_Layer[i];
  }

NJ = OShFunc * NEl + 1;

std::cout << " Total number of elements= " << NEl << std::endl;
std::cout << " Total number of joints  = " << NJ << std::endl;

std::cout << " End InputArrays " << std::endl;

}

