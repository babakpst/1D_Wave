

#include "../include/discretize_the_domain_cls.h"

// constructor
main_ns::discretization_ns::discretization_cls::
    discretization_cls(const main_ns::model_ns::model_cls *InputModel) : model(InputModel)
{

  std::cout << " -allocating arrays for discretization ..." << std::endl;
  // Allocating required 1D arrays - vectors
  MTel = new int[model->NEl]; // Material Type of Elements

  // Allocating required 2D arrays - matrices
  INod = new int *[model->NNode]; // node connectivity
  for (int i = 0; i < model->NNode; i++)
  {
    INod[i] = new int[model->NEl];
  }

  ID = new int *[model->NJ]; // Identifications
  for (int i = 0; i < model->NJ; i++)
  {
    ID[i] = new int[model->NDOF];
  }

  XYZ = new double *[model->NJ]; // Coordinates
  for (int i = 0; i < model->NJ; i++)
  {
    XYZ[i] = new double[model->NDim];
  }

  Nodal_History = new int[model->Dis_History];

  NoBndry_DRM = new int[model->NNBndry]; // vector that holds the node numbers on the DRM boundary-
                                         // There is only one node on the DRM boundary
  NoLayer_DRM = new int[model->NNLayer]; // vector that holds the node numbers on the DRM layer.
                                         // There are only 2 nodes in the layer in a 1D wave motion
}

/*
###################################################################################################
Purpose: This function discretized the domain, creates elements, assigns material properties, etc.

Developed by: Babak Poursartip
 
The Institute for Computational Engineering and Sciences (ICES)
The University of Texas at Austin
================================= V E R S I O N ===================================================
V0.00: 05/20/2018 - Subroutine initiated.
V0.01: 05/21/2018 - Initiated: Compiled without error for the first time.

###################################################################################################
*/
void main_ns::discretization_ns::discretization_cls::Discretization()
{

  // = Variables ==================================================================================

  // - Integer variables --------------------------------------------------------------------------
  int i, j, ij;  // loop variable
  int iel;       // loop variable
  int Mat_index; // material counter
  int NJ_Count;  // counter on the number of joints
  int NJ_Layer;  // number of joints in the layer
  int NEl_Layer; // number of elements in the layer

  // - Real variables -----------------------------------------------------------------------------
  double h;          // element size
  double Coordinate; // Holds the coordinate of the node
  double Level;      //
  double tol;

  // - Strings ------------------------------------------------------------------------------------
  std::string TempS; // Temporary variable for reading strings from input files

  // ==================== Code ====================================================================
  std::cout << " -discretization ..." << std::endl;

  // create the coordinates of nodes
  std::cout << " -coordinates ..." << std::endl;
  NJ_Count = 0;
  XYZ[0][0] = -(model->L);
  for (i = 0; i < model->NMat; i++)
  {
    NEl_Layer = model->Element_Layer[i];
    NJ_Layer = model->OrderOfShapeFunc * NEl_Layer + 1;
    h = 0.50 * model->Layer_Depth[i] / NEl_Layer;
    h = (floor(h * 10000)) / 10000;
    if (i == 0)
      Level = -(model->L);
    else
      Level = model->Length[i - 1];

    for (ij = 1; ij < NJ_Layer - 2; ij++)
    { // mind the error in saving real numbers
      NJ_Count++;
      XYZ[NJ_Count][0] = Level + ij * h;
    }
    NJ_Count++;
    XYZ[NJ_Count][0] = model->Length[i] - 0.5 * (model->Length[i] - XYZ[NJ_Count - 1][0]);
    NJ_Count++;
    XYZ[NJ_Count][0] = model->Length[i];
  }

  if ((model->NJ - 1) != NJ_Count)
  {
    std::cout << " Fatal Error: refer to the discretization function." << std::endl;
    std::cout << " Number of joints should be: " << model->NJ - 1 << " but it is: "
              << NJ_Count << std::endl;
    return;
  }

  // connectivity - Note: Node numbers start from zero
  std::cout << " -connectivities ..." << std::endl;
  for (iel = 0; iel < model->NEl; iel++)
  {
    if (model->OrderOfShapeFunc == 1)
    {
      INod[0][iel] = iel;
      INod[1][iel] = iel + 1;
    }
    else if (model->OrderOfShapeFunc == 2)
    {
      INod[0][iel] = iel * 2 + 0;
      INod[1][iel] = iel * 2 + 1;
      INod[2][iel] = iel * 2 + 2;
    }
  }

  // constraints
  std::cout << " -equation numbers ..." << std::endl;
  for (i = 0; i < model->NJ; i++)
  {
    for (j = 0; j < model->NDim; j++)
    {
      ID[i][j] = 0;
    }
  }

  // computing equation number
  NEqM = -1;
  for (i = 0; i < model->NJ; i++)
  {
    for (j = 0; j < model->NDOF; j++)
    {
      NEqM++;
      ID[i][j] = NEqM;
      // We already know that no node has been fixed. Equation number then starts from zero.
    }
  }
  NEqM++;

  // material property of element
  std::cout << " -material properties of elements..." << std::endl;
  Mat_index = 0;
  for (iel = 0; iel < model->NEl; iel++)
  {
    Coordinate = XYZ[INod[2][iel]][0];
    if (Coordinate <= model->Length[Mat_index])
      MTel[iel] = Mat_index;
    else
    {
      Mat_index++;
      MTel[iel] = Mat_index;
    }
  }

  // Finding the DRM node numbers

  //NoBndry_DRM[0] = (int)(DRM_Loc /h );
  //NoLayer_DRM[0] = NoBndry_DRM [0] - 2;
  //NoLayer_DRM[1] = NoBndry_DRM [0] - 1;

  NoBndry_DRM[0] = 2 * model->NEl_DRM;
  NoLayer_DRM[0] = NoBndry_DRM[0] - 2;
  NoLayer_DRM[1] = NoBndry_DRM[0] - 1;

  tol = 0.0001;

  // Finding out the node numbers for recording the history of displacement
  for (i = 0; i < model->Dis_History; i++)
  {
    for (ij = 0; ij < model->NJ; ij++)
    {
      if ((model->Loc_History[i] - tol < XYZ[ij][0]) && (XYZ[ij][0] < model->Loc_History[i] + tol))
      {
        Nodal_History[i] = ij;
        break;
      }
      else if (model->Loc_History[i] < XYZ[ij][0])
      {
        Nodal_History[i] = ij;
        break;
      }
    }
  }

  std::cout << " Done with discretization, successfully. " << std::endl
            << std::endl;
}
