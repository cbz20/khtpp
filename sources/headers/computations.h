// 
// This file is part of kht++.
// 
// kht++ is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as 
// published by the Free Software Foundation, either version 3 of 
// the License, or (at your option) any later version.
// 
// kht++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General 
// Public License along with kht++.  If not, see 
// <https://www.gnu.org/licenses/>.
// 



std::cout << ">>> computations for -c" << c_opt << ":\n" << std::flush;

Complex<CobObj,CobMor,VALUE> cx;
#ifdef rationals
std::cout << "!!!!!!!! WARNING !!!!!!!!\n"
          << "The implementation of rational number is still in an experimental state.\n"
          << "You should not trust the output, especially for large computations, since\n"
          << "the program does not check for integer overflows.\n"
          << "!!!!!!!! WARNING !!!!!!!!\n"
          << std::flush;
cx = T0.CobComplex<VALUE>();
# else
if ( coeffs.size() > 1 )
{
     cx = to_coeff<CobObj,CobMor,VALUE> ( cxZ );
     cx.cancel();
} else {
     cx = T0.CobComplex<VALUE>();
};
#endif


if ( o_flag ) {
    std::string twists {cx.optimize ( T0.bot_orient() ) };
    T0=T0.add ( twists );
    if ( twists.empty() == false )
    {
        conditional_append (
            file.fullname() + ".kht",
            "% With options '" +
            fn_options +
            "', add these twists to optimize: " +
            twists );
    };
};

if ( T0.n_ended ( 2 ) )
{
     // T0 is a 1-1- or 2-0-tangle.
     T0.to_svg ( metadata,file.fullname() + "/" + file.name() + fn_options );
     T0.to_svg ( metadata,file.fullname() + "/" + file.name() + fn_options );
     auto cxBNr = cx.to_BN();
     cxBNr.check();
     cxBNr.cancel(); ///> \todo figure out why this is necessary sometimes (example: examples/strong_inversions/9_39.kht )
     cxBNr.clean_up();
     cxBNr.check();
     cxBNr.print (
          file.fullname() + "/cxCKh" + fn_options,
          metadata,
          false );
     //
} else if ( T0.n_ended ( 4 ) )
{
     // T0 is a 1-3-, 2-2-, or 3-1-tangle;
     auto cxBNr=cx.to_BN();
     cxBNr.check();
     //cxBN.to_precurve().to_svg ( file.fullname() + "/precurve_original" );
     cxBNr.clean_up ( max_clean_up_iter );
     cxBNr.check();
     T0.to_svg ( metadata,file.fullname() + "/" + file.name() + fn_options );
     cxBNr.print (
          file.fullname() + "/cxBNr" + fn_options,
          metadata );
     auto cxKhr = cxBNr.cone ( 1 );
     cxKhr.clean_up ( max_clean_up_iter );
     cxKhr.check();
     cxKhr.print (
          file.fullname() + "/cxKhr" + fn_options,
          metadata,
          true,true );// experimental feature for detecting curves and slopes; however, the feature is buggy, since there can be rational components in Khr that look like special components of length 1 (= rational components with 2-dim local system)
//           true,
//           true);
     //
//      cxBN.print ( file.fullname() + "/cxBN" );
//      cxBN.clean_up_once(0);
//      cxBN.print ( file.fullname() + "/cxBN1" );
//      cxBN.clean_up_once(1);
//      cxBN.print ( file.fullname() + "/cxBN2" );
//      cxBN.clean_up_once(0);
//      cxBN.print ( file.fullname() + "/cxBN3" );
//      cxBN.clean_up_once(1);
//      cxBN.print ( file.fullname() + "/cxBN4" );
//      cxBN.clean_up_once(0);
//      cxBN.print ( file.fullname() + "/cxBN5" );
//      if(cxBN.is_loop_type(0)){
//           std::cout << "complex is loop-type wrt S\n";
//      };
//      if(cxBN.is_loop_type(1)){
//           std::cout << "complex is loop-type wrt D\n";
//      };
//      std::cout << "Printed the computed type D structure into " << file.fullname() << "/cxBN.\n";
//      //
//      auto precurve1=cxBN.to_precurve();
//      if ( precurve1.check() ==false ) {
//           std::cout << "!!!WARNING!!!\nprecurve is not well-defined.";
//      };
//      std::cout << "\n\noriginal\n" << precurve1.to_string();
//      precurve1.to_svg ( file.fullname() + "/precurve1_original" );
//      precurve1.sort ( 0 );
//      if ( precurve1.check() ==false ) {
//           std::cout << "!!!WARNING!!!\nprecurve is not well-defined.";
//      };
//      std::cout << "\n\nafter sort(0)\n" << precurve1.to_string();
//      precurve1.to_svg ( file.fullname() + "/sort0" );
//      for ( int i = 1; i<5; i++ ) {
//           precurve1.sort ( i );
//           precurve1.to_svg ( file.fullname() + "/sort"+std::to_string ( i ) );
//      }
     //  std::cout << precurve1.to_string();
     //
} else
{
     cx.print (
          file.fullname() + "/cx" + fn_options,
          metadata );
     std::cout << "Printed the computed type D structure into " << file.fullname() << "/cx" + fn_options + ".\n";
};
#undef VALUE
break;
