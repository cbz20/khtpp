
auto chains1 = Chains<VALUE>( File (summand1) );
auto chains2 = Chains<VALUE>( File (summand2) );
if ( sum_non_compact_flag ){
    std::cout << "Extracting only non-compact curves.\n";
    chains1 = chains1.non_compacts();
    chains2 = chains2.non_compacts();
};
auto cx1 = Complex<BNObj,BNMor,VALUE>(chains1);
auto cx2 = Complex<BNObj,BNMor,VALUE>(chains2);

std::cout << "First summand:\n"
          << cx1.to_chains().to_string()
          << "Second summand:\n"
          << cx2.to_chains().to_string()
          << "Computing the tangle sum. ";
auto sum = cx1+cx2;
sum.check();
std::cout << "The resulting complex has "
          << sum.get_objects().size()
          << " generators. Now cancelling.\n";
sum.cancel();
sum.check();
sum.clean_up();
std::cout << "Tangle sum:\n"
          << sum.to_chains().to_string();
#undef VALUE
break;
