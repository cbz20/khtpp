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



#ifndef TESTS_TPP
#define TESTS_TPP
#ifndef TESTS_H
#include "../headers/Tests.h"
#endif


/// \file Tests.cpp
/// \brief implements some tests for checking some features of the program

/// prints \c message and ok/failed depending on whether \c exp is true or false
void test ( std::string msg, bool exp )
{
     std::cout << "\n" << msg << ": ";
     if ( exp ) {
          std::cout << "ok\n";
     } else {
          std::cout << "failed\n";
     };
};

template<typename Coeff>
bool Test_ChainConversion( const int &details ){
     //0 means terse output
     //1 means more details
     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "%%% BEGIN Test_ChainConversion %%%\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "\n";
     std::string in {"h^6 q^18 δ^3 ⬯~~S~>"};
//      auto i1 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^3 ⬮——D—>";
//      auto i2 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮~~(-S^3)~>";
//      auto i3 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮<—(-43.D^3)——";
//      auto i4 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮~~(-43.S)~>";
//      auto i5 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮~>";
//      auto i6 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮<~";
//      auto i7 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮—>";
//      auto i8 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮<—";
//      auto i9 {Clink<Coeff>( in )};
//      in = "h^-6 q^-21 δ^-9/2 ⬮";
//      auto i10 {Clink<Coeff>( in )};
//      
//      in = "h^ 5 q^18 δ^   4 ⬮~>⬮—>⬮~>⬯<—⬯<~⬮<—";
//      auto i11 {Chain<Coeff>( in )};
//      in = "h^ 5 q^18 δ^   4 ⬯~>⬮—>⬮~>⬯<—⬯<~⬮<—";
//      auto i12 {Chain<Coeff>( in )};
//      in = "h^ -2 q^ -4 δ^  0 ⬯~~S^4~>⬯—>⬯~>⬮—>⬮<~S^3~~⬯<—⬯";
//      auto i13 {Chain<Coeff>( in )};
     
     auto c1 {Chains<Coeff> (File("chains_test_input1"))};
     auto cx1 = Complex<BNObj,BNMor,Coeff>(c1.non_compacts());
     std::cout << cx1.to_chains().to_string();
     auto c2 {Chains<Coeff> (File("chains_test_input2"))};
     auto cx2 = Complex<BNObj,BNMor,Coeff>(c2.non_compacts());
     std::cout << cx2.to_chains().to_string();
//      for (auto &clink : c1.chains.front().clinks ){
//           std::cout << clink.to_string()
//                     << ": morphism goes from "
//                     << clink.morphism.get_front().to_string()
//                     << " to "
//                     << clink.morphism.get_back().to_string()
//                     << ".\n";
//                     
//      };
//      for (auto &clink : c2.chains.front().clinks ){
//           std::cout << clink.to_string()
//                     << ": morphism goes from "
//                     << clink.morphism.get_front().to_string()
//                     << " to "
//                     << clink.morphism.get_back().to_string()
//                     << ".\n";
//                     
//      };
//      std::cout << c1.to_string() << "\n";
//      std::cout << "compacts:\n" << c1.compacts().to_string();
//      std::cout << "non-compacts:\n" << c1.non_compacts().to_string();
     
     auto cx3 = cx1+cx2;
//      std::cout << cx3.to_string();
     cx3.check();
     cx3.cancel();
     cx3.check();
//      std::cout << cx3.to_string();
     cx3.clean_up();
     std::cout << cx3.to_chains().to_string();

     return true;
};

//bool CobNonZero (const int& row, const int& col, const CobMor& value) {
//  return value.nonZero();
//};


/*
template<typename Coeff>
bool Test_Coefficients(const int &details){
  //0 means terse output
  //1 means more details
  std::cout << "\n"
    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
    "%%% BEGIN Test_Coefficients %%%\n"
    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
    "\n";
  Coeff a{1};
  Coeff b{2};
  std::cout << "a+b=1+2=" << (a+b)
	    << " mod " << which_P(Coeff(0)) << ".\n";
  std::cout << "b*b=2*2=" << (b*b)
    	    << " mod " << which_P(Coeff(0)) << ".\n";
  std::cout << "(-1)*b=" << ((-1)*b)
    	    << " mod " << which_P(Coeff(0)) << ".\n";
  std::cout << "1/b=" << 1/b
    	    << " mod " << which_P(Coeff(0)) << ".\n";

  Q r{126,15};
  Q s{-6};
  std::cout << "126/15.reduce()=" << r.reduce() << ".\n";
  std::cout << "r+(-6/1)=" << r+s << ".\n";
  std::cout << "(-1)/r=" << (-1)/r << ".\n";

  std::cout << "\n"
    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
    "%%% END Test_Coefficients %%%\n"
    "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
    "\n";
};
*/


template<typename Coeff>
bool Test_Cob ( const int &details )
{
     //0 means terse output
     //1 means more details
     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%%%\n"
               "%%% BEGIN Test_Cob %%%\n"
               "%%%%%%%%%%%%%%%%%%%%%%\n"
               "\n";

/*

     // basic objects
    if ( details==1 ) {
          std::cout << "******basic (1,3)-tangles\n";
          std::cout << "\n***b.print():\n";
          b1.print();
          std::cout << "\n***c.print():\n";
          c1.print();
          CobObj c_hqq = c1;

          c_hqq.shift_hq ( 1,2 );
          std::cout << "\n***c_hqq.print():\n";
          c_hqq.print();
     };
     */
     int number_of_strands {4};
     std::cout << "Testing init of PCA::vec:" << PCA::vec.size() << std::flush;
     expand_vec_if_needed( number_of_strands );
     std::cout << "arcs representing all tangles with up to "
               << number_of_strands
               << " strands: \n";
     for ( auto a : PCA::vec ){
          std::cout << "[";
          for (auto b : a ){
               std::cout << "[ ";
               for ( auto c : b.first ){
                    std::cout << int(c) << " ";
               };
               std::cout << "|";
               std::cout << b.second;
               std::cout << "]";
          };
          std::cout << "]\n\n";    
     };
     std::cout << "Testing init of gens:" << PCA::gens.size() << std::flush;
     expand_gens_if_needed( number_of_strands );
     std::cout << "arcs representing all tangles with up to "
               << number_of_strands
               << " strands: \n";

     int counter {0};
     for ( auto arcss : PCA::gens ){
          counter = 0;
          std::cout << "[";
          for (auto a : arcss ){
               ++counter;
               std::cout << "[ ";
               a.print();
               std::cout << "]";
          };
          std::cout << "]\ntotal="
                    << counter
                    << "\n";    
     };
     std::cout << " comps lookup-tables:\n";
     for ( auto mat : PCA::comps ){
          for ( int front=0 ;front<mat.cols(); ++front ){
               for ( int back=0 ;back<mat.rows(); ++back ){
                    std::cout << stringLL(mat.coeff(back,front));
               };
               std::cout << "\n";
          };
          std::cout << "\n";
     };
     
     std::cout << " addCup lookup-tables:\n";
     for ( auto strands_no : PCA::addCup ){
          std::cout << "[";
          for (auto cap_no : strands_no ){
               std::cout << "[ ";
               for ( auto e : cap_no ){
                    std::cout << e << " ";
               };
               std::cout << "]";
          };
          std::cout << "]\n\n";    
     };
     std::cout << " AddCupGivesClosedComponent lookup-tables:\n";
     for ( auto a : PCA::addCupGCC ){
          std::cout << stringLL(a) << "\n\n";
     };
     std::cout << " addCap lookup-tables:\n";
     for ( auto strands_no : PCA::addCap ){
          std::cout << "[";
          for (auto cap_no : strands_no ){
               std::cout << "[ ";
               for ( auto e : cap_no ){
                    std::cout << e << " ";
               };
               std::cout << "]";
          };
          std::cout << "]\n\n";
     };
     
     auto arcs = Arcs(5);
     arcs.print();
     arcs.rotate( 5 );
     arcs.print();
     auto parallel = CobObj(1);
     parallel.print();
     

     // basic morphisms
     Deco<Coeff> deco0 {0,{0},1};
     Deco<Coeff> deco1 {1,{0,0},-1};
     Deco<Coeff> deco2 {0,{0,1},1};
     Deco<Coeff> deco3 {0,{1,0},1};


     std::list<Deco<Coeff>> decos {deco2, deco1, deco3};
     CobMor<Coeff> zero_mor = CobMor<Coeff> ( 1 );
     CobMor<Coeff> S_bc=CobMor<Coeff> ( 2,2,0,1, {deco0} );
     CobMor<Coeff> S_cb=CobMor<Coeff> ( 2,2,1,0, {deco0} );
     //  S_bc.print();
     //  S_cb.print();
     std::cout << "\n******multiplication in Cob\n"  <<std::flush;
     CobMor<Coeff> SS = S_bc * S_cb;
     std::cout << "\n****** after multiplication in Cob\n" <<std::flush;
     CobMor<Coeff> ExpectedResult = CobMor<Coeff> ( 2,2,1,1,decos );
     test ( "saddle*saddle",SS==ExpectedResult );
     if ( details==1 ) {
          S_bc.print();
          S_cb.print();
          SS.print();
     };

/*

     CobMor<Coeff> D=CobMor<Coeff> ( b1,b1, {deco2} );
     CobObj clt1 {1,5,{1,0,3,2,5,4},0,0};
     CobObj clt2 {1,5,{3,2,1,0,5,4},0,0};
     CobMor<Coeff> cob1 {clt1,clt2,{deco1,deco2}};
     CobMor<Coeff> cob2 {clt1,clt2,{deco1,deco3},{{4,5},{0,1,2,3}}};
     // cob2 = cob1, just the order of components is different.

     std::cout << "\n******addition in Cob\n";
     CobMor<Coeff> cob3 = cob1+cob2;
     ExpectedResult=CobMor<Coeff> ( clt1,clt2, {Deco<Coeff> ( 0,{1,0},1 ),Deco<Coeff> ( 0,{0,1},1 ) } );
     test ( "cob1+cob2",cob3==ExpectedResult );
     if ( details==1 ) {
          cob3.print();
     };
     // test("0+cob1",0+cob1==cob1);
     //test("cob1+0",cob1+0==cob1);
     CobMor<Coeff> cob4 = -cob1;
     ExpectedResult=CobMor<Coeff> ( clt1,clt2, {Deco<Coeff> ( 1,{0,0},1 ),Deco<Coeff> ( 0,{0,1},-1 ) } );
     test ( "-cob1",cob4==ExpectedResult );
     if ( details==1 ) {
          cob4.print();
     };*/

//      std::cout << "\nTesting AddCup/AddCap:\n";
//      std::cout << "b1:\n";
//      b1.print();
//      std::cout << "b1.AddCap(1):\n";
     //b1.AddCap(1);
     //b1.print();
     /*
     std::cout << "b.AddCap(2):\n";
     b2.AddCap(2);
     b2.print();
     std::cout << "b.AddCap(3):\n";
     b3.AddCap(3);
     b3.print();
     std::cout << "b.AddCap(4):\n";
     b4.AddCap(4);
     b4.print();
     //
     auto c1{c};
     c1.AddCap(1);
     auto S_b1c1=S_bc.AddCap(b1,c1,1);
     std::cout << "S_bc.AddCap(b1,c1,1):\n";
     S_b1c1.print();
     //
     auto B1{b};
     auto B2{b};
     std::cout << "b.AddCup(1):";
     B1.AddCup(1);
     std::cout << "Closed component? (1=yes)"
         << b.AddCupGivesClosedComponent(1) << "\n";
     B1.print();
     std::cout << "b.AddCup(2):";
     B2.AddCup(2);
     std::cout << "Closed component? (1=yes)"
         << b.AddCupGivesClosedComponent(2) << "\n";
     B2.print();



     std::cout << "\nTesting conversion Cob->BN:\n";
     std::cout << "b: ";
     b.to_BNObj().print();
     std::cout << "c: ";
     c.to_BNObj().print();
     std::cout << "\nS_bc: ";
     S_bc.to_BNMor().print();
     std::cout << "\nSS: ";
     SS.to_BNMor().print();
     std::cout << "\nD: ";
     D.to_BNMor().print();
     std::cout << "\nDD: ";
     (D*D).to_BNMor().print();
     std::cout << "\nS_bc*D=0: ";
     (S_bc*D).to_BNMor().print();
     std::cout << "\n13.H=13.D-13.SS: ";
     CobMor<Coeff>(b,b,{Deco<Coeff>(1,{0,0},13)}).to_BNMor().print();
     std::cout << "\n13.H^2=13.D^2+13.SSSS: ";
     CobMor<Coeff>(b,b,{Deco<Coeff>(2,{0,0},13)}).to_BNMor().print();

     std::cout << "\n\nTesting multiplication with scalars:\n";
     BNMor<Coeff> D_BNcopy=D.to_BNMor();
     if (details==1){
       D.print();
       std::cout << "D*=6: ";
     };
     D*=6;
     if (details==1){
       D.print();
     };
     D_BNcopy*=6;
     if (details==1){
       D_BNcopy.print();
     };

     std::cout << "\nTesting is_id:";
     test("is_id",CobMor<Coeff>(b,b,{Deco<Coeff>(0,{0,0},1)}).is_id());
     test("is_id",CobMor<Coeff>(b,b,{Deco<Coeff>(0,{0,0},1)}).to_BNMor().is_id());

     std::cout << "\nTesting conversion of coefficients in BNAlgebra:\n";
     if (details==1){
       std::cout << "over Z: ";
       D_BNcopy.print();
     };
     auto D_BN_mod_5=D_BNcopy.template to_coeffs<Z_mod<5>>();
     if (details==1){
       std::cout << "\nover Z/5Z: ";
       D_BN_mod_5.print();
     };
       std::cout << "\nTesting conversion of coefficients in Cob:\n";
     if (details==1){
       std::cout << "over Z: ";
       D.print();
     };
     auto D_mod_5=D.template to_coeffs<Z_mod<5>>();
       //  auto D_mod_5_alt=D.to_coeffs_alt2(Z_mod<5>(0));
       //  auto D_mod_5_alt=D.template to_coeffs_alt<Z_mod<5>>();
     if (details==1){
       std::cout << "\nover Z/5Z: ";
       D_mod_5.print();
       //  D_mod_5_alt.print();
     };
     */

     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%\n"
               "%%% END Test_Cob %%%\n"
               "%%%%%%%%%%%%%%%%%%%%\n"
               "\n";
     return 1;
}


template <typename Coeff>
bool Test_Complex ( const int &details )
{
     //0 means terse output
     //1 means more details
     
     /*
     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "%%% BEGIN Test_Complex %%%\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "\n";
     typedef Eigen::Triplet<CobMor<Coeff>> T;

     // COMPLEX 0
     std::cout << "\n\n   COMPLEX 1\n\n";
     std::vector<T> tripletList0;

     Eigen::SparseMatrix<CobMor<Coeff>> diff0 ( 1,1 );
     diff0.setFromTriplets ( tripletList0.begin(), tripletList0.end() );


     // COMPLEX 1
     std::cout << "\n\n   COMPLEX 1\n\n";
     std::vector<T> tripletList1;
     tripletList1.reserve ( 2 );
     tripletList1.push_back ( T ( 1,0,CobMor<Coeff> ( b1,c1, {Deco<Coeff> ( 0,{0},1 ) } ) ) );
     tripletList1.push_back ( T ( 2,1,CobMor<Coeff> ( c1,c1, {Deco<Coeff> ( 0,{1,0},-1 ),Deco<Coeff> ( 0,{0,1},1 ) } ) ) );

     Eigen::SparseMatrix<CobMor<Coeff>> diff1 ( 3,3 );
     diff1.setFromTriplets ( tripletList1.begin(), tripletList1.end() );
     Complex<CobObj,CobMor,Coeff> cx1 {{b1,c1,c1},diff1 };
     if ( details==1 ) {
          cx1.print();
//           std::cout << "print only the component from 1 to 0:";
//           cx1.print ( 1,0 );
//           std::cout << "print only the component from 0 to 1:";
//           cx1.print ( 0,1 );
     };
     std::cout << "cx1=[b---S-->c---D-->c]:";
     test ( "cx1.check()",cx1.check() );

     // COMPLEX 2
     std::cout << "\n\n   COMPLEX 2\n\n";
     std::vector<T> tripletList2;
     tripletList2.reserve ( 4 );
     tripletList2.push_back ( T ( 1,0,CobMor<Coeff> ( b1,c1, {Deco<Coeff> ( 0,{0},1 ) } ) ) );
     tripletList2.push_back ( T ( 3,1,CobMor<Coeff> ( c1,b1, {Deco<Coeff> ( 0,{0},1 ) } ) ) );
     tripletList2.push_back ( T ( 2,0,CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{1,0},-1 ),Deco<Coeff> ( 0,{0,1},-1 ),Deco<Coeff> ( 1,{0,0},1 ) } ) ) );
     tripletList2.push_back ( T ( 3,2,CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{0,0},1 ) } ) ) );

     Eigen::SparseMatrix<CobMor<Coeff>> diff2 ( 4,4 );
     diff2.setFromTriplets ( tripletList2.begin(), tripletList2.end() );
     Complex<CobObj,CobMor,Coeff> cx2 {{b1,c1,b1,b1},diff2 };
     if ( details==1 ) {
          cx2.print();
     };
     */
//      std::cout << "Testing AddCap(1)\n";
//      auto cx2AddCap1 = cx2.AddCap( 0 );
//      if ( details==1 ) {
//           cx2AddCap1.print();
//      };
//      std::cout << "cx2=[square composed of b---S-->c---S-->c and b---1-->b---H-->c]:";
//      test ( "cx2.check()",cx2.check() );
//
//      std::cout << "\nTesting conversion Cob->BN:\n";
//      Complex<BNObj,BNMor,Coeff> cx1BN=to_BN ( cx1 );
//      if ( details==1 ) {
//           cx1BN.print();
//      };
//      Complex<BNObj,BNMor,Coeff> cx2BN=to_BN ( cx2 );
//      if ( details==1 ) {
//           cx2BN.print();
//      };
//
//      std::cout << "Cancel differential 2--->3.";
//      cx2BN.cancel ( 2,3 );
//      if ( details==1 ) {
//           cx2BN.print();
//      };
//
//      // COMPLEX 3
//      std::cout << "\n\n   COMPLEX 3\n\n";
//      std::vector<T> tripletList3;
//      tripletList3.reserve ( 3 );
//      tripletList3.push_back ( T ( 1,0,CobMor<Coeff> ( c1,b1, {Deco<Coeff> ( 0,{0},3 ) } ) ) );
//      tripletList3.push_back ( T ( 1,2,CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{0,0},-1 ) } ) ) );
//      tripletList3.push_back ( T ( 3,2,CobMor<Coeff> ( b1,c1, {Deco<Coeff> ( 0,{0},2 ) } ) ) );
//
//      Eigen::SparseMatrix<CobMor<Coeff>> diff3 ( 4,4 );
//      diff3.setFromTriplets ( tripletList3.begin(), tripletList3.end() );
//      Complex<CobObj,CobMor,Coeff> cx3 { {c1,b1,b1,c1},diff3 };
//      if ( details==1 ) {
//           cx3.print();
//      };
//      std::cout << "cx3=[square composed of c---3.S-->b<--(-1)---b---2.S-->c]:";
//      test ( "cx3.check()",cx3.check() );
//
//      std::cout << "\nTesting conversion Cob->BN:\n";
//      Complex<BNObj,BNMor,Coeff> cx3BN=to_BN ( cx3 );
//      if ( details==1 ) {
//           cx3BN.print();
//      };
//      std::cout << "Convert -> Cancel:";
//      cx3BN.cancel();
//      if ( details==1 ) {
//           cx3BN.print();
//      };
//      std::cout << "Cancel -> Convert:";
//      cx3.cancel();
//      if ( details==1 ) {
//           to_BN ( cx3 ).print();
//      };
//
//      // COMPLEX 4
//      std::cout << "\n\n   COMPLEX 4\n\n";
//      std::vector<T> tripletList4;
//      tripletList4.reserve ( 3 );
//      tripletList4.push_back ( T ( 1,0,CobMor<Coeff> ( c1,b1, {Deco<Coeff> ( 0,{0},3 ) } ) ) );
//      tripletList4.push_back ( T ( 2,0,CobMor<Coeff> ( c1,b1, {Deco<Coeff> ( 0,{0},1 ) } ) ) );
//      tripletList4.push_back ( T ( 3,1,CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{1,0},4 ),Deco<Coeff> ( 0,{0,1},-4 ) } ) ) );
//
//      Eigen::SparseMatrix<CobMor<Coeff>> diff4 ( 4,4 );
//      diff4.setFromTriplets ( tripletList4.begin(), tripletList4.end() );
//      Complex<CobObj,CobMor,Coeff> cx4 { {c1,b1,b1,b1},diff4 };
//      if ( details==1 ) {
//           cx4.print();
//      };
//      std::cout << "cx4=[square\n"
//                << "c---3.S-->b---4.D-->b\n"
//                << " ---2.S-->b          ]:";
//      test ( "cx4.check()",cx4.check() );
//
//      std::cout << "\nTesting isotopy along 2---(10.id)-->1:\n";
//      Complex<BNObj,BNMor,Coeff> cx4BN=to_BN ( cx4 );
//      if ( details==1 ) {
//           cx4BN.print();
//      };
//      BNMor<Coeff> isotopy1=CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{0,0},10 ) } ).to_BNMor();
//      cx4BN.isotopy ( 2,1,isotopy1 );
//      if ( details==1 ) {
//           cx4BN.print();
//      };
//
//      // COMPLEX 5
//      std::cout << "\n\n   COMPLEX 5\n\n";
//      std::vector<T> tripletList5;
//      tripletList5.reserve ( 3 );
//      tripletList5.push_back ( T ( 0,1,CobMor<Coeff> ( b1,c1, {Deco<Coeff> ( 0,{0},3 ) } ) ) );
//      tripletList5.push_back ( T ( 0,2,CobMor<Coeff> ( b1,c1, {Deco<Coeff> ( 0,{0},1 ) } ) ) );
//      tripletList5.push_back ( T ( 1,3,CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{1,0},4 ),Deco<Coeff> ( 0,{0,1},-4 ) } ) ) );
//
//      Eigen::SparseMatrix<CobMor<Coeff>> diff5 ( 4,4 );
//      diff5.setFromTriplets ( tripletList5.begin(), tripletList5.end() );
//      Complex<CobObj,CobMor,Coeff> cx5 { {c1,b1,b1,b1},diff5 };
//      if ( details==1 ) {
//           cx5.print();
//      };
//      std::cout << "cx5=[square\n"
//                << "c<--3.S---b<--4.D---b\n"
//                << " <--2.S---b          ]:";
//      test ( "cx5.check()",cx5.check() );
//
//
//      Complex<BNObj,BNMor,Coeff> cx5BN=to_BN ( cx5 );
//      if ( details==1 ) {
//           cx5BN.print();
//      };
//      std::cout << "\nTesting conversion to Z_mod<3>:\n";
//      auto cx5BN_mod3=to_coeff<BNObj,BNMor,Z_mod<3>> ( cx5BN );
//      if ( details==1 ) {
//           cx5BN_mod3.print();
//      };
//      std::cout << "\nTesting isotopy along 2---(10.id)-->1:\n";
//      BNMor<Coeff> isotopy2=CobMor<Coeff> ( b1,b1, {Deco<Coeff> ( 0,{0,0},10 ) } ).to_BNMor();
//      cx4BN.isotopy ( 2,1,isotopy2 );
//
//      if ( details==1 ) {
//           cx5BN.print();
//      };
//
//      // COMPLEX 6
//      std::cout << "\n\n   COMPLEX 6\n\n";
//      std::vector<T> tripletList6;
//      tripletList6.reserve ( 4 );
//      tripletList6.push_back ( T ( 1,0,CobMor<Coeff> ( c1,b1, {Deco<Coeff> ( 0,{0},4 ) } ) ) );
//      tripletList6.push_back ( T ( 2,0,CobMor<Coeff> ( c1,c1, {Deco<Coeff> ( 1,{0,0},-2 ),Deco<Coeff> ( 0,{1,0},2 ),Deco<Coeff> ( 0,{0,1},2 ) } ) ) );
//      tripletList6.push_back ( T ( 3,1,CobMor<Coeff> ( b1,c1, {Deco<Coeff> ( 0,{0},-1 ) } ) ) );
//      tripletList6.push_back ( T ( 3,2,CobMor<Coeff> ( c1,c1, {Deco<Coeff> ( 0,{0,0},2 ) } ) ) );
//
//      Eigen::SparseMatrix<CobMor<Coeff>> diff6 ( 4,4 );
//      diff6.setFromTriplets ( tripletList6.begin(), tripletList6.end() );
//      Complex<CobObj,CobMor,Coeff> cx6 { {c1,b1,c1,c1},diff6 };
//      if ( details==1 ) {
//           cx6.print();
//      };
//      std::cout << "cx6=[square\n"
//                << "c---4.S-->b-(-1).S->c\n"
//                << " --2.SS-->c--2.id--> ]:";
//      test ( "cx6.check()",cx6.check() );
//      Complex<BNObj,BNMor,Coeff> cx6BN = to_BN ( cx6 );
//      if ( details==1 ) {
//           cx6BN.print();
//      };
//
//      std::cout << "\nTesting conversion from Z to Z/3Z:\n";;
//      auto cx6BN_mod3=to_coeff<BNObj,BNMor,Z_mod<3>> ( cx6BN );
//      if ( details==1 ) {
//           cx6BN_mod3.print();
//      };
//      cx6BN_mod3.cancel();
//      if ( details==1 ) {
//           cx6BN_mod3.print();
//      };
//
//      // COMPLEX 7
//      std::cout << "\n\n   COMPLEX 7\n\n";
//      std::vector<T> tripletList7;
//      tripletList7.reserve ( 1 );
//      tripletList7.push_back ( T ( 1,0,CobMor<Coeff> ( c1,c1, {Deco<Coeff> ( 2,{0,1},4 ) } ) ) );
//
//      Eigen::SparseMatrix<CobMor<Coeff>> diff7 ( 2,2 );
//      diff7.setFromTriplets ( tripletList7.begin(), tripletList7.end() );
//      auto c_shifted=c1;
//      c_shifted.shift_hq ( 1,1 );
//      Complex<CobObj,CobMor,Coeff> cx7 { {c1,c_shifted},diff7 };
//      if ( details==0 ) {
//           cx7.print();
//      };
//      auto cx7_1=AddCup ( cx7,1 );
//      if ( details==0 ) {
//           cx7_1.print();
//      };
//      std::cout << "Before pairing on the right with )(:";
//      to_BN ( cx7 ).print();
//      std::cout << "Before pairing on the right with )(:";
//
//      std::cout << "\n ⤲   ⤺ ⤮⤭\n↑ ↑ ↑ ↑";
     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "%%% END Test_Complex %%%\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "\n";
     return true;
}
//  auto cx7_2=AddCap(cx7_1,1);
//cx7_2.print();
// to_BN(cx7_2).print();


//std::cout << "cx7=[c---4.id-->c]:";
//Complex<BNObj,BNMor<Coeff>> cx7BN=to_BN(cx7);
//if (details==0){
//  cx7BN.print();
//};

//TODO check isolating arrows; make sure that we are working over a field, otherwise we might run into trouble!!!
// TODO implement randomized arrow pushing algorithm
//std::cout << "\nTesting isolating arrow 0----->2:\n";
//isolate_arrow(cx4BN,0,2);

// Tests from main file
//
//
//
//to_coeff<BNObj,BNMor,Z_mod<2>>(cx.to_BN()).isolate_arrow(18,38);
//cx.to_BN().isotopy(2,6,BNMor<int>(BNObj(),BNObj(),{Label<int>(1,3)}));
//to_coeff<BNObj,BNMor,Z_mod<2>>(cx.to_BN()).print();

/*
    Complex<CobObj,CobMor<int>> min_cx=Complex<CobObj,CobMor<int>>({CobObj(1,1,{1,0})},{});
    std::cout << "\nminimal complex:\n";
    //  min_cx.print();
    std::cout << "\ncx1:\n";
    auto cx1=AddCap(min_cx,2);
    //  cx1.print();
    std::cout << "\ncx2:\n";
    auto cx2=AddY(cx1,1,0);
    //  cx2.print();
    std::cout << "\ncx3:\n";
    auto cx3=AddY(cx2,1,0);
    //  cx3.print();
    //std::cout << "\ncx4:\n";
    //auto cx4=AddY(cx3,1,0);
    //cx4.print();

    for (int k=0;k<60;++k){
      std::cout << "\nIteration " << k <<":\n";
      cx3=AddY(cx3,1,0);
      std::cout << "size before cancellation: " <<cx3.get_objects().size() << "\n";
      cx3.cancel();
      //cx3.print();
      std::cout << "size after cancellation:  " <<cx3.get_objects().size() << "\n";
    };
    */

/*
if(cx.check()){
  std::cout << "Complex well-defined.\n";
}
else{
  std::cout << "ERROR: Complex not well-defined.";
};
*/


//
//
//
//
//
/*
template<typename Coeff>
bool Test_Precurves(const int &details)
{
    //0 means terse output
    //1 means more details
    std::cout << "\n"
              "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
              "%%% BEGIN Test_Precurves %%%\n"
              "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
              "\n";
    if (false) {
        // curve 1
        std::vector<Halfgen> halfgenLB{{{0,0,0},0,0,{0,1,0},-1},
            {{0,1,0},0,2,{0,0,0},1}};
        std::vector<Halfgen> halfgenRB{{{0,0,1},0,0,{1,0,1},-1},
            {{0,1,1},0,2,{0,1,1},0}};
        std::vector<Halfgen> halfgenRC{{{1,0,1},0,0,{0,0,1},1}};
        std::vector<Halfgen> halfgenLC{{{1,0,0},0,0,{1,0,0},0}};
        auto arcB{ArcN<Coeff>(halfgenLB,
                              halfgenRB,
        {{1,0,12}},{{0,1,3},{0,0,4}},
        {{0,1}})};
        auto arcC{ArcN<Coeff>(halfgenLC,
                              halfgenRC,
        {},{},
        {{0}})};
        auto precurve{Precurve<Coeff>({arcB,arcC})};

        std::cout << precurve.to_string();
        if(precurve.check()==false) {
            std::cout << "!!!WARNING!!!\nprecurve is not well-defined.";
        };
        precurve.sort(1);
        std::cout << precurve.to_string();
        if(precurve.check()==false) {
            std::cout << "!!!WARNING!!!\nprecurve is not well-defined.";
        };
    };
    if (true) {
        // curve 2
        std::vector<Halfgen> halfgenLB{{{0,0,0},0,0,{0,0,0},0},
            {{0,1,0},0,2,{0,2,0},-1},
            {{0,2,0},0,4,{0,1,0},1}};
        std::vector<Halfgen> halfgenRB{{{0,0,1},0,0,{1,2,1},1},
            {{0,1,1},0,2,{1,1,1},-1},
            {{0,2,1},0,4,{1,0,1},-1}};
        std::vector<Halfgen> halfgenRC{{{1,0,1},0,0,{0,2,1},1},
            {{1,1,1},0,0,{0,1,1},1},
            {{1,2,1},0,0,{0,0,1},-1},
            {{1,3,1},0,0,{0,0,1},0}};
        std::vector<Halfgen> halfgenLC{{{1,0,0},0,0,{1,3,0},-1},
            {{1,1,0},0,0,{1,2,0},-1},
            {{1,2,0},0,0,{1,1,0},1},
            {{1,3,0},0,0,{1,0,0},1}};
        auto arcB{ArcN<Coeff>(halfgenLB,
                              halfgenRB,
        {{1,0,12},{1,2,11}},{{0,2,3},{0,0,4},{1,0,12},{0,1,6}},
        {{0,1,2}})};
        auto arcC{ArcN<Coeff>(halfgenLC,
                              halfgenRC,
        {{1,0,6},{1,3,7},{2,2,5}},{{1,0,12},{1,3,2},{0,0,9}},
        {{0,1,2,3}})};
        auto precurve{Precurve<Coeff>({arcB,arcC})};
        if(precurve.check()==false) {
            std::cout << "!!!WARNING!!!\nprecurve is not well-defined.";
        };

        std::cout << "\n\noriginal\n" << precurve.to_string();
        precurve.to_svg("precurve2_original");
        precurve.sort(0);
        std::cout << "\n\nafter sort (0)\n" << precurve.to_string();
        precurve.to_svg("precurve2_sort0");
        precurve.sort(1);
        std::cout << "\n\nafter sort (1)\n" << precurve.to_string();
        precurve.to_svg("precurve2_sort1");
        precurve.sort(2);
        std::cout << "\n\nafter sort (2)\n" << precurve.to_string();
        precurve.to_svg("precurve2_sort2");
        if(precurve.check()==false) {
            std::cout << "!!!WARNING!!!\nprecurve is not well-defined.";
        };
    };

    std::cout << "\n"
              "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
              "%%% END Test_Precurves %%%\n"
              "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
              "\n";
    return true;
};*/


/*

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//          AlgebraTest
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class Algebra // AlgebraTest
{
private:

public:
  std::string str;
  Algebra(std::string str);
  Algebra(int i);
  Algebra();

  bool nonZero() const;
  void operator+= (const Algebra &alg2){*this=(*this+alg2);}
  friend bool operator== (const Algebra &alg1,const Algebra &alg2);
  friend Algebra operator+ (const Algebra &alg1, const Algebra &alg2);
  friend Algebra operator* (const Algebra &alg2, const Algebra &alg1);
};

Algebra::Algebra(int i) :
  str(std::to_string(i)){
  std::cout << "Calling Algebra(" << i << ").\n";
}

Algebra::Algebra() :
  str("()"){
  std::cout << "Calling Algebra().\n";
}

Algebra::Algebra(std::string str) :
  str(str)
{
  std::cout << "Calling Algebra(" << str << ").\n";
}

bool operator== (const Algebra &alg1,const Algebra &alg2){
  std::cout << "Computing " << alg1.str << "==" << alg2.str << ".\n";
  return (alg1.str==alg2.str);
}

Algebra operator+ (const Algebra &alg1,const Algebra &alg2)
{
  std::cout << "Computing " << alg1.str << "+" << alg2.str << ".\n";
  std::string output=alg1.str+"+"+alg2.str;
  return Algebra(output);
}

bool Algebra::nonZero() const{
  return this->str!="0";
};

Algebra operator*(const Algebra &alg2, const Algebra &alg1){
  std::cout << "Computing " << alg2.str << "*" << alg1.str << ".\n";
  std::string output=alg2.str+"*"+alg1.str;
  return Algebra(output);
}

bool SparseMatrixTestAlgebra(){
  typedef Eigen::SparseMatrix<Algebra> SpMat;
  typedef Eigen::Triplet<Algebra> T;
  std::vector<T> tripletList;
  tripletList.reserve(4);
  tripletList.push_back(T(0,0,Algebra("a")));
  tripletList.push_back(T(0,1,Algebra("b")));
  tripletList.push_back(T(1,0,Algebra("c")));
  tripletList.push_back(T(1,1,Algebra("d")));
  SpMat mat(3,3);
  mat.setFromTriplets(tripletList.begin(), tripletList.end());
  std::cout << "\nmatrix mat: \n"
	    << mat.coeff(0,0).str << "|"
	    << mat.coeff(0,1).str << "\n"
	    << mat.coeff(1,0).str << "|"
	    << mat.coeff(1,1).str << "\n\n";


  SpMat mat2(3,3);
  mat2=mat*mat;
  std::cout << "\nmatrix mat2: \n"
	    << mat2.coeff(0,0).str << "|"
	    << mat2.coeff(0,1).str << "\n"
	    << mat2.coeff(1,0).str << "|"
	    << mat2.coeff(1,1).str << "\n\n";
  return true;
};
*/





template<typename Coeff>
bool Test_PrecomputedAlgebra ( const int &details )
{
     //0 means terse output
     //1 means more details
     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "%%% BEGIN Test_PrecomputedAlgebra %%%\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "\n";

//      std::cout << "\n\n   Catalan tangles\n\n";
//      std::vector<std::vector<CobObj>> Catalantangles;
//      std::vector<CobObj> temp;
//      for ( TE i = 1; i < 10 ; ++i ) {
//           temp = {};
//           std::cout << CatalanTangles ( i ).size() << "\n";
//           for ( const auto &e : CatalanTangles ( i ) ) {
//                temp.push_back ( CobObj ( 2*i,0,e ) );
//           };
//           Catalantangles.push_back ( temp );
//      };
     
     std::cout << "\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "%%% END Test_PrecomputedAlgebra %%%\n"
               "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
               "\n";
     return true;
};



// template specialization

#define VALUE int
#include "../specialization/Tests.h"
#define VALUE Z_mod<2>
#include "../specialization/Tests.h"
#define VALUE Z_mod<3>
#include "../specialization/Tests.h"
#define VALUE Z_mod<5>
#include "../specialization/Tests.h"
#define VALUE Z_mod<7>
#include "../specialization/Tests.h"
#define VALUE Z_mod<custom_coeff>
#include "../specialization/Tests.h"
#define VALUE Z_mod<integer_simulant>
#include "../specialization/Tests.h"
#define VALUE Q
#include "../specialization/Tests.h"

#endif
