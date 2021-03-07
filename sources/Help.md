  usage: kht++ [options] [<filename1> <filename2> ... <filenameN>] 
  
  options:
    -a, --allcoeff        Computes the invariants over all available
                          coefficients.
    -c<i>, --coeff <i>    Computes invariants with coefficients in a
                          field k specified by the integer <i>:
  
                                  i = 2   k = Z/2 (default)
                                      3       Z/3
                                      5       Z/5
                                      7       Z/7
                                     11       Z/11
                                      0       Q (experimental)
  
                          The program can also compute the invariants
                          over k = Z/<p> for any prime <p>. For this,
                          recompile the program after setting
  
                                  custom_coeff = <p>
  
                          in the source file '/headers/constants.h' and 
                          run then program with option -c<p>. 
    -d, --double          Doubles all tangle strands before computing
                          the invariants of the tangles.
    -h, --help            Displays this help message.
    -o, --optimize        Attempts to minimize the number of generators
                          in the invariants by adding twists to the 
                          tangle ends.
    -q, --quotient        Computes the invariants of the quotient tangle 
                          of a strongly invertible knot specified by a 
                          symmetric diagram. The tangle string in the 
                          corresponding .kht-file should have the prefix 
                          's.'. 
    -r <name>:<p>/<q>, --rational <name>:<p>/<q> 
                          Computes all symmetric diagrams of the 2-bridge
                          knot corresponding to the fraction <p>/<q> and 
                          saves them in one or more files
                                                                   
                            <name>.kht   or   <name>-{1,2}.kht
                                                                   
                          in the directory 
                                                                   
                            examples/strong_inversions/2-bridge-knots
                                                                   
    -s, --simplify        Attempts to simplify the diagram before
                          computing any invariants.
    -t, --test            Runs some tests.
    -v, --version         Returns the version of the program and exits.
    -w, --web             Prints the tangle input, (re)compiles and opens
                          the html pages for the specified file(s) using
                          all previously computed data.
                                                                        
  examples:
  1) kht++ (without arguments) 
     Allows the user to define a tangle and compute its invariants. The
     tangle is stored in a .kht-file for later use.
  
  2) kht++ -w <filename>
     Recompiles .html-file and opens it in browser.
  
  3) kht++ -c3 -c5 <filename1> <filename2> <filename3>
     Computes the tangle invariants for the tangles stored in files
     <filename1>, <filename2>, and <filename3> with coefficients in the
     fields of three and five elements. 
  
  Please report bugs to: cbz20@posteo.net
