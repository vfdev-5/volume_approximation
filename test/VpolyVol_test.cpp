// VolEsti (volume computation and sampling library)

// Copyright (c) 20012-2018 Vissarion Fisikopoulos
// Copyright (c) 2018 Apostolos Chalkis

// Licensed under GNU LGPL.3, see LICENCE file

#include "doctest.h"
#include <unistd.h>
#include "Eigen/Eigen"
#include "volume.h"
#include "polytope_generators.h"
#include <typeinfo>

template <typename NT>
NT factorial(NT n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

template <typename NT, class RNGType, class Polytope>
void test_volume(Polytope &VP, NT expected, NT tolerance=0.1)
{

    typedef typename Polytope::PolytopePoint Point;

    // Setup the parameters
    int n = VP.dimension();
    int walk_len=10 + n/10;
    int nexp=1, n_threads=1;
    NT e=1, err=0.0000000001;
    int rnum = std::pow(e,-2) * 400 * n * std::log(n);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    RNGType rng(seed);
    boost::normal_distribution<> rdist(0,1);
    boost::random::uniform_real_distribution<>(urdist);
    boost::random::uniform_real_distribution<> urdist1(-1,1);

    vars<NT, RNGType> var(rnum,n,walk_len,n_threads,err,e,0,0,0,0,0.0,rng,
                          urdist,urdist1,-1.0,false,false,false,false,false,false,true,false);

    //Compute chebychev ball//
    std::pair<Point,NT> CheBall;

    // Estimate the volume
    std::cout << "Number type: " << typeid(NT).name() << std::endl;
    NT vol = 0;
    unsigned int const num_of_exp = 10;
    CheBall = VP.ComputeInnerBall();
    Polytope VP2 = VP;
    for (unsigned int i=0; i<num_of_exp; i++)
    {

        VP.init(n, VP2.get_mat(), VP2.get_vec());
        vol += volume(VP,var,CheBall);
    }
    NT error = std::abs(((vol/num_of_exp)-expected))/expected;
    std::cout << "Computed volume (average) = " << vol/num_of_exp << std::endl;
    std::cout << "Expected volume = " << expected << std::endl;
            CHECK(error < tolerance);
}



template <typename NT>
void call_test_cube(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef boost::mt19937    RNGType;
    typedef VPolytope<Point, RNGType > Vpolytope;
    Vpolytope P;

    std::cout << "--- Testing volume of V-cube3" << std::endl;
    P = gen_cube<Vpolytope>(3, true);
    test_volume<NT, RNGType>(P, 8.0);
    if(typeid(NT)== typeid(double)) {
        std::cout << "--- Testing volume of V-cube4" << std::endl;
        P = gen_cube<Vpolytope>(4, true);
        test_volume<NT, RNGType>(P, 16.0);

    }
}

template <typename NT>
void call_test_cross(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef boost::mt19937    RNGType;
    typedef VPolytope<Point, RNGType > Vpolytope;
    Vpolytope P;

    std::cout << "--- Testing volume of V-cross3" << std::endl;
    P = gen_cross<Vpolytope>(3, true);
    test_volume<NT, RNGType>(P, 1.333333);

    std::cout << "--- Testing volume of V-cross4" << std::endl;
    P = gen_cross<Vpolytope>(4, true);
    test_volume<NT, RNGType>(P, 0.6666667);

}

template <typename NT>
void call_test_simplex() {
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef boost::mt19937    RNGType;
    typedef VPolytope<Point, RNGType > Vpolytope;
    Vpolytope P;

    std::cout << "--- Testing volume of V-simplex4" << std::endl;
    P = gen_simplex<Vpolytope>(4, true);
    test_volume<NT, RNGType>(P, 1.0 / factorial(4.0));

    std::cout << "--- Testing volume of V-simplex5" << std::endl;
    P = gen_simplex<Vpolytope>(5, true);
    test_volume<NT, RNGType>(P, 1.0 / factorial(5.0));

}

TEST_CASE("cube") {
    call_test_cube<double>();
    //call_test_cube<float>();
    //call_test_cube<long double>();
}

TEST_CASE("cross") {
    call_test_cross<double>();
    //call_test_cross<float>();
    //call_test_cross<long double>();
}

TEST_CASE("simplex") {
    call_test_simplex<double>();
    //call_test_simplex<float>();
    //call_test_simplex<long double>();
}
