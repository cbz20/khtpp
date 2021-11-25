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
                          over k = Z/<p> for any prime <p>.  For this,
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
    -P <s>,<o>, --prepend <s>,<o> 
                          Prepends a tangle string <s> with orientation
                          information <o> for the top tangle ends, after 
                          processing all other options.  For example, 
                          setting  <s> = r1  and  <o> = 1,1  adds a cup 
                          at the top of a tangle which will then have 
                          two upwards oriented tangle ends at the top. 
                          This option can only be called once. 
    -p <s>, --append <s>  Appends a tangle string <s>, after processing
                          all other options.  For example, <s> = x0.x0 
                          adds two extra twists at the bottom of the 
                          tangle. This option can be called any number
                          of times. 
    -q, --quotient        Computes the invariants of the quotient tangle 
                          of a strongly invertible knot specified by a 
                          symmetric diagram.  The tangle string in the 
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
    --sum <s1>:<s2>       Computes the complex of the tangle sum of two 
                          Conway tangles (ie the tangle obtained by 
                          stacking the first tangle on top of the other)
                          from previously computed curves. 
                          <s1> and <s2> specify the full relative paths 
                          of the text file containing the curves for the 
                          first and second tangle, respectively. Such
                          files are stored by the program in separate 
                          folders next to the standard .kht-files. 
                          However, any text file can be used, as long as 
                          its format agrees with the output format for 
                          curves; see 'Output' in the documentation.
    --sumnc <s1>:<s2>     Same as '--sum <s1>:<s2>', except that only 
                          the non-compact curves are taken as input. 
                          This option is useful if one is only interested
                          in the non-compact curve of a tangle sum, since
                          it runs much faster than '--sum <s1>:<s2>' if 
                          the input curves contain many compact curves.
    -t, --test            Runs some tests.
    -v, --version         Returns the version of the program and exits.
    -w, --web             Prints the tangle input, (re)compiles and opens
                          the html pages for the specified file(s) using
                          all previously computed data.
                                                                        
  examples:
  1) kht++ (without arguments) 
     Allows the user to define a tangle and compute its invariants.  The
     tangle is stored in a .kht-file for later use.
  
  2) kht++ -w <filename>
     Recompiles .html-file and opens it in browser.
  
  3) kht++ -c3 -c5 <filename1> <filename2> <filename3>
     Computes the tangle invariants for the tangles stored in files
     <filename1>, <filename2>, and <filename3> with coefficients in the
     fields of three and five elements. 
  
  Please report bugs to: cbz20@posteo.net
