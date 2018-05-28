
/*
Purpose: This class defines all virutal functions related to all solvers.
*/

namespace main_ns
{

namespace solver_ns
{

class Solver_cls{

  public:
  virtual void matrices_fn (void) =0;
  virtual void assemble_fn (void) =0;
  virtual void shapefunctions_fn (void) =0;
  virtual void load_fn (void) =0;
  virtual void solver_fn (void) =0;
  virtual void results_fn (void) =0;



}



}