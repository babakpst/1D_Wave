#include "../include/create_skyline_matrices_cls.h"

// Constructor: we also create and allocate matrices
main_ns::Matrices_ns::Matrices_Skyline_cls::Matrices_Skyline_cls(main_ns::discretization_ns::discretization_cls *aDiscretization,
                                                                 main_ns::model_ns::model_cls *aModel) : main_ns::Matrices_ns::Matrices_cls(aDiscretization, aModel)
{
  main_ns::Matrices_ns::Matrices_Skyline_cls::allocating_global_matrices_fn();
  main_ns::Matrices_ns::Matrices_Skyline_cls::allocating_local_matrices_fn();
  main_ns::Matrices_ns::Matrices_Skyline_cls::Skyline_fn(); 
}

/*
###################################################################################################
Purpose: This function allocates global matrices. In this module we consider full matrices.

Developed by: Babak Poursartip
 
The Institute for Computational Engineering and Sciences (ICES)
The University of Texas at Austin	
================================= V E R S I O N ===================================================
V0.00: 06/18/2018 - Subroutine initiated.


###################################################################################################
*/

void main_ns::Matrices_ns::Matrices_Skyline_cls::allocating_global_matrices_fn()
{

  std::cout << " -allocating global matrices ..." << std::endl;

  JD = new int[DiscretizedModel->NEqM];
  NTK = new int[DiscretizedModel->NEqM];

  K_S = new double[JD[DiscretizedModel->NEqM - 1]]; // Stiffness Matrix
  C_S = new double[JD[DiscretizedModel->NEqM - 1]]; // Damping matrix
  M_S = new double[JD[DiscretizedModel->NEqM - 1]]; // Mass matrix

  F = new double[DiscretizedModel->NEqM];





















  std::cout << " -initializing global matrices ..." << std::endl;
  for (int i = 0; i < DiscretizedModel->NEqM; i++)
  {
    for (int j = 0; j < DiscretizedModel->NEqM; j++)
    {
      M[i][j] = 0.0;
      C[i][j] = 0.0;
      K[i][j] = 0.0;
    }
    F[i] = 0.0;
  }

  std::cout << " -allocating DRM matrices ..." << std::endl;
  K_eb = new double *[Model->NDim * Model->NNLayer]; //
  for (int i = 0; i < (Model->NDim * Model->NNLayer); i++)
  {
    K_eb[i] = new double[Model->NDim * Model->NNBndry];
  }

  C_eb = new double *[Model->NDim * Model->NNLayer]; //
  for (int i = 0; i < (Model->NDim * Model->NNLayer); i++)
  {
    C_eb[i] = new double[Model->NDim * Model->NNBndry];
  }

  M_eb = new double *[Model->NDim * Model->NNLayer]; //
  for (int i = 0; i < (Model->NDim * Model->NNLayer); i++)
  {
    M_eb[i] = new double[Model->NDim * Model->NNBndry];
  }

  ND_b = new int[Model->NNBndry * Model->NDim];
  ND_e = new int[Model->NNLayer * Model->NDim];

  // Filling the index for layered nodes
  for (int i = 0; i < Model->NNLayer; i++)
  {
    for (int j = 0; j < Model->NDim; j++)
    {
      ND_e[j * Model->NNLayer + i] = DiscretizedModel->ID[DiscretizedModel->NoLayer_DRM[i]][j];
    }
  }

  // Filling the index for boundary nodes
  for (int i = 0; i < Model->NNBndry; i++)
  {
    for (int j = 0; j < Model->NDim; j++)
    {
      ND_b[j * Model->NNBndry + i] = DiscretizedModel->ID[DiscretizedModel->NoBndry_DRM[i]][j];
    }
  }

  std::cout << " Done with allocation, successfully." << std::endl;
}

/*
###################################################################################################
Purpose: This function assembles local matrices into the full matrices.

Developed by: Babak Poursartip
 
The Institute for Computational Engineering and Sciences (ICES)
The University of Texas at Austin	
================================= V E R S I O N ===================================================
V0.00: 06/18/2018 - Subroutine initiated.

###################################################################################################
*/

void main_ns::Matrices_ns::Matrices_Skyline_cls::assemble_local_to_global_fn()
{

  int l, n, i, j, ij;

  for (l = 0; l < NEqEl; l++)
  {
    for (n = 0; n < NEqEl; n++)
    {
      i = ND[l];
      j = ND[n];
      if (i > j)
        continue;
      ij = JD[j] + i - j;

      K_S[ij] = K_S[ij] + Ke[l][n];
      C_S[ij] = C_S[ij] + Ce[l][n];
      M_S[ij] = M_S[ij] + Me[l][n];
    }
  }
}



/*
###################################################################################################
Purpose: This function creates the required matrices for the skyline method.

Developed by: Babak Poursartip
 
The Institute for Computational Engineering and Sciences (ICES)
The University of Texas at Austin	
================================= V E R S I O N ===================================================
V0.00: 06/18/2018 - Subroutine initiated.

###################################################################################################
*/

void main_ns::Matrices_ns::Matrices_Skyline_cls::Skyline_fn()
{

int i,j,k;

  for (int i = 0; i < NEqEl; i++){
    ND[i]=0.0;
  }

  for (int i=0; i < DiscretizedModel->NEqM; i++){
    NTK[i] = i ;
  }

  for (int iel=0; iel < Model->NEl; iel++){ 
    for (int i = 0; i<Model->NNode; i++){
      k = DiscretizedModel->INod[i][iel];
        for (int j=0; j < Model->NDOF; j++){
          ND[j * Model->NNode + i] = DiscretizedModel->ID[k][j] ;
        }
    }

    for (int l =0; l<NEqEl; l++){
      for (int k=0; k<NEqEl; k++){ 
        i = ND[l];
        j = ND[k];
        //if ((i==0) ||  (j==0)) continue;
        if (i>j) continue;
        if (i<NTK[j]) NTK[j]=i;
      }
    }
  }

JD[0]=0;

  for (int i=1; i<DiscretizedModel->NEqM; i++){ 
    JD[i]=JD[i-1]+i+1-NTK[i];
  }

std::cout << "End function skyline" << std::endl; 

}

/*
//***************************************************************************************************************************************************
// Copy the submatrices for DRM loads.  
//***************************************************************************************************************************************************
void DRM_Matrices_Skyline( int & NNBndry, int &NNLayer, double *& K_S, double *& C_S, double *& M_S, double **& K_eb, double **& C_eb, double **& M_eb, int *&ND_e, int *&ND_b , int *& JD)  
{

int ij,i,j,l,n;   // Loop indices

// - Code ---------------------------------------------------------------------
std::cout << "Create DRM matrices ..." << endl; 

  for ( l = 0; l < NNLayer * NDim; l++) {
    for ( n = 0; n < NNBndry * NDim; n++) {

      i = ND_e [ l ] ;
      j = ND_b [ n ] ;


      if (i>j) {
        ij = i;
        i  = j;
        j  = ij; 
        std::cout << " i and j replaced"<< endl;
      }

      ij = JD[j]+i-j;

//cout<< "DRM" << ij << " K_S  "<< K_S[ ij ] <<  " M_S "<< M_S[ ij ] << " C_S  "<< C_S[ ij ] <<  endl;
   
      K_eb[l][n] = K_S[ ij ];
      C_eb[l][n] = C_S[ ij ];
      M_eb[l][n] = M_S[ ij ];

//      cin.get();
    }
  }

std::cout << "DRM matrices created." << std::endl; 

}
*/






