/* 
 * VMMLib - Tensor Classes
 *  
 * @author Susanne Suter
 * @author Jonas Boesch
 *
 * The tucker3 tensor class is consists of the same components (core tensor, basis matrices u1-u3) as the tucker3 model described in:
 * - Tucker,  “Some mathematical notes on three-mode factor analysis”, Psychometrika, vol. 31, no. 3, pp. 279–311., 1966 Sep.
 * - De Lathauwer L., De Moor B., Vandewalle J., ``A multilinear singular value decomposition'', 
 * SIAM J. Matrix Anal. Appl., vol. 21, no. 4, Apr. 2000, pp. 1253-1278.
 * - De Lathauwer L., De Moor B., Vandewalle J., ``On the Best rank-1 and Rank-$(R_1, R_2, ..., R_N)$ Approximation and Applications of Higher-Order Tensors'', 
 * SIAM J. Matrix Anal. Appl., vol. 21, no. 4, Apr. 2000, pp. 1324-1342.
 * - T. G. Kolda and B. W. Bader. Tensor Decompositions and Applications. 
 * SIAM Review, Volume 51, Number 3, Pages 455-500, September 2009.
 * 
 */

#ifndef __VMML__TUCKER3_TENSOR__HPP__
#define __VMML__TUCKER3_TENSOR__HPP__

#include <vmmlib/tensor3.hpp>
#include <vmmlib/tensor3_iterator.hpp>
#include <vmmlib/lapack_svd.hpp>
#include <vmmlib/matrix_pseudoinverse.hpp>

namespace vmml
{
	
	template< size_t J1, size_t J2, size_t J3, size_t I1, size_t I2, size_t I3, typename T = float >
	class tucker3_tensor
	{
public:    
    tucker3_tensor( tensor3< J1, J2, J3, T >& core, matrix< I1, J1, T >& U1, matrix< I2, J2, T >& U2, matrix< I3, J3, T >& U3 );

	typedef tensor3< I1, I2, I3, T > t3_type;
	typedef typename t3_type::iterator t3_iterator;
	typedef typename t3_type::const_iterator t3_const_iterator;

	typedef tensor3< J1, J2, J3, T > t3_core_type;
	typedef typename t3_core_type::iterator t3_core_iterator;
	typedef typename t3_core_type::const_iterator t3_core_const_iterator;
		
	typedef matrix< I1, J1, T > u1_type;
	typedef typename u1_type::iterator u1_iterator;
	typedef typename u1_type::const_iterator u1_const_iterator;

	typedef matrix< I2, J2, T > u2_type;
	typedef typename u2_type::iterator u2_iterator;
	typedef typename u2_type::const_iterator u2_const_iterator;
	
	typedef matrix< I3, J3, T > u3_type;
	typedef typename u3_type::iterator u3_iterator;
	typedef typename u3_type::const_iterator u3_const_iterator;
		
	//TODO typedef for m_lateral_type, m_frontal_type, and m_horizontal_type;
	typedef matrix< I1, I2*I3, T > mode1_matricization_type;
	typedef matrix< I2, I1*I3, T > mode2_matricization_type;
	typedef matrix< I3, I1*I2, T > mode3_matricization_type;

		
	void set_core( const t3_core_type& core )  { _core = core; } ;
	void set_u1( const u1_type& U1 ) { _u1 = U1; } ;
	void set_u2( const u2_type& U2 ) { _u2 = U2; } ;
	void set_u3( const u3_type& U3 ) { _u3 = U3; } ;
	
    void get_core( t3_core_type& data_ ) const { data_ = _core; } ;
	void get_u1( u1_type& U1 ) const { U1 = _u1; } ;
	void get_u2( u2_type& U2 ) const { U2 = _u2; } ;
	void get_u3( u3_type& U3 ) const { U3 = _u3; } ;
		
	void export_to( std::vector< T >& data_ ) const;
	void import_from( std::vector< T >& data_ );	
	
	void reconstruction( t3_type& data_ ) const;
	void decomposition( const t3_type& data_ ); 
		
	/* derive core
	   implemented accodring to core = data x_1 U1_pinv x_2 U2_pinv x_3 U3_pinv, 
	   where x_1 ... x_3 are n-mode products and U1_pinv ... U3_pinv are inverted basis matrices
	   the inversion is done with a matrix pseudoinverse computation
	 */
    void derive_core( const t3_type& data_, t3_core_type& core_, const u1_type& U1_, const u2_type& U2_, const u3_type& U3_ );
	//faster: but only if basis matrices are orthogonal
	void derive_core_orthogonal_bases( const t3_type& data_, t3_core_type& core_, const u1_type& U1_, const u2_type& U2_, const u3_type& U3_ );

	/*	higher-order singular value decomposition (HOSVD) with full rank decomposition (also known as Tucker decomposition). 
		see: De Lathauer et al, 2000a: A multilinear singular value decomposition. 
		the hosvd can be computed (a) with n-mode PCA, i.e., an eigenvalue decomposition on the covariance matrix of every mode's matricization, and 
		(b) by performing a 2D SVD on the matricization of every mode. Matrix matricization means that a tensor I1xI2xI3 is unfolded/sliced into one matrix
		with the dimensions I1xI2I3, which corresponds to a matrizitation alonge mode I1.
		other known names for HOSVD: n-mode SVD, 3-mode factor analysis (3MFA, tucker3), 3M-PCA, n-mode PCA, higher-order SVD
	 */
	void hosvd( const t3_type& data_ );
	void hosvd_on_eigs( const t3_type& data_ );
	void hosvd_mode1( const t3_type& data_, u1_type& U1_ ) const;
	void hosvd_mode2( const t3_type& data_, u2_type& U2_ ) const;
	void hosvd_mode3( const t3_type& data_, u3_type& U3_ ) const;

		
	/*	higher-order orthogonal iteration (HOII) is a truncated HOSVD decompositions, i.e., the HOSVD components are of lower-ranks. An optimal rank-reduction is 
		performed with an alternating least-squares (ALS) algorithm, which minimizes the error between the approximated and orignal tensor based on the Frobenius norm
		see: De Lathauwer et al, 2000b; On the best rank-1 and rank-(RRR) approximation of higher-order tensors.
		the HOII can be computed based on (a) n-mode PCA, i.e., an eigenvalue decomposition on the covariance matrix of every mode's matriciziation, and 
		(b) by performing a 2D SVD on the matricization of every mode. Matrix matricization means that a tensor I1xI2xI3 is unfolded/sliced into one matrix
		with the dimensions I1xI2I3, which corresponds to a matrizitation alonge mode I1.
	 */
	void hoii( const t3_type& data_ );
		
	void optimize_mode1( const t3_type& data_, tensor3< I1, J2, J3, T >& projection_, const u2_type& U2_, const u3_type& U3_ ) const;
	void optimize_mode2( const t3_type& data_, tensor3< J1, I2, J3, T >& projection_, const u1_type& U1_, const u3_type& U3_ ) const;		
	void optimize_mode3( const t3_type& data_, tensor3< J1, J2, I3, T >& projection_, const u1_type& U1_, const u2_type& U2_ ) const;
	
		
	void tucker_als( const t3_type& data_ );	
		
	template< size_t K1, size_t K2, size_t K3>
	void reduce_ranks( const tucker3_tensor< K1, K2, K3, I1, I2, I3, T>& other ); //call TuckerJI.reduce_ranks(TuckerKI) K1 -> J1, K2 -> J2, K3 -> J3

	template< size_t K1, size_t K2, size_t K3>
	void subsampling( const tucker3_tensor< J1, J2, J3, K1, K2, K3, T>& other, const T& factor  );

	template< size_t K1, size_t K2, size_t K3>
	void subsampling_on_average( const tucker3_tensor< J1, J2, J3, K1, K2, K3, T>& other, const T& factor  );

	template< size_t K1, size_t K2, size_t K3>
	void region_of_interest( const tucker3_tensor< J1, J2, J3, K1, K2, K3, T>& other, 
							const T& start_index1, const T& end_index1, 
							const T& start_index2, const T& end_index2, 
							const T& start_index3, const T& end_index3);
	
	
private:
    t3_core_type _core ;
	u1_type _u1 ;
	u2_type _u2 ;
	u3_type _u3 ;
	
}; // class tucker3_tensor


#define VMML_TEMPLATE_STRING    	template< size_t J1, size_t J2, size_t J3, size_t I1, size_t I2, size_t I3, typename T >
#define VMML_TEMPLATE_CLASSNAME     tucker3_tensor< J1, J2, J3, I1, I2, I3,  T >


VMML_TEMPLATE_STRING
VMML_TEMPLATE_CLASSNAME::tucker3_tensor( t3_core_type& core, u1_type& U1, u2_type& U2, u3_type& U3 )
: _core(core), _u1(U1), _u2(U2), _u3(U3)
{
	//assert(J1 <= I1);
	//assert(J2 <= I2);
	//assert(J3 <= I3);	
}

VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::reconstruction( t3_type& data_ ) const
{
	data_.full_tensor3_matrix_multiplication( _core, _u1, _u2, _u3 );
}


VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::decomposition( const t3_type& data_ )
{
	tucker_als( data_ );
		
}

VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::tucker_als( const t3_type& data_ )
{
	hoii( data_ );
	
	//derive core
	derive_core_orthogonal_bases( data_, _core, _u1, _u2, _u3 );
}


VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::hosvd_mode1( const t3_type& data_, u1_type& U1_ ) const
{
	mode1_matricization_type m_lateral; // -> u1
	data_.lateral_matricization( m_lateral);
	
	//std::cout << "hosvd mode1, m_lateral: " << std::endl << m_lateral << std::endl;
	
	vector< I2*I3, double > lambdas_u1;

	lapack_svd< I1, I2*I3, double > svd1;
	if( svd1.compute_and_overwrite_input( m_lateral, lambdas_u1 ))
		m_lateral.get_sub_matrix( U1_ );
	else 
		U1_.zero();
}

	
VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::hosvd_mode2( const t3_type& data_, u2_type& U2_ ) const
{
	mode2_matricization_type m_frontal; // -> u2
	data_.frontal_matricization( m_frontal);
	//std::cout << "hosvd mode2, m_frontal: " << std::endl << m_frontal << std::endl;
	
	vector< I1*I3, double > lambdas_u2;

	lapack_svd< I2, I1*I3, double > svd2;
	if( svd2.compute_and_overwrite_input( m_frontal, lambdas_u2 ))
		m_frontal.get_sub_matrix( U2_ );
	else 
		U2_.zero();
}



VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::hosvd_mode3( const t3_type& data_, u3_type& U3_ ) const
{
	mode3_matricization_type m_horizontal; //-> u3
	data_.horizontal_matricization( m_horizontal);
	//std::cout << "hosvd mode3, m_horizontal: " << std::endl << m_horizontal << std::endl;
	
	vector< I1*I2, double > lambdas_u3;
	lapack_svd< I3, I1*I2, double > svd3;
	if( svd3.compute_and_overwrite_input( m_horizontal, lambdas_u3 ))
		m_horizontal.get_sub_matrix( U3_ );
	else 
		U3_.zero();
}


VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::hosvd( const t3_type& data_ )
{	
	hosvd_mode1( data_, _u1 );
	hosvd_mode2( data_, _u2 );
	hosvd_mode3( data_, _u3 );
}


VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::hosvd_on_eigs( const t3_type& data_ )
{
	//matricization along each mode (backward matricization after Lathauwer et al. 2000a)
	mode1_matricization_type m_lateral; // -> u1
	mode2_matricization_type m_frontal; // -> u2
	mode3_matricization_type m_horizontal; //-> u3
	data_.lateral_matricization( m_lateral);
	data_.frontal_matricization( m_frontal);
	data_.horizontal_matricization( m_horizontal);
	
	//std::cout << "tensor input for tucker, method1: " << std::endl << tensor_ << std::endl;
	
	//2-mode PCA for each matricization A_n: (1) covariance matrix, (2) SVD
	//covariance matrix S_n for each matrizitation A_n
	matrix< I1, I1, T > s1;
	matrix< I2, I2, T > s2;
	matrix< I3, I3, T > s3;
	
	s1.multiply(m_lateral, transpose(m_lateral));
	s2.multiply(m_frontal, transpose(m_frontal));
	s3.multiply(m_horizontal, transpose(m_horizontal));
	
	/*std::cout << "covariance matrix s1: " << std::endl << s1 << std::endl 
	 << "covariance matrix s2: " << s2 << std::endl
	 << "covariance matrix s3: " << s3 << std::endl;*/
	
	//eigenvalue decomposition for each covariance matrix
	
	
}

	
VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::optimize_mode1( const t3_type& data_, tensor3< I1, J2, J3, T >& projection_, const u2_type& U2_, const u3_type& U3_ ) const
{
	//compute pseudo inverse for matrices u2,u3
	u2_type u2_pinv_t ;
	u3_type u3_pinv_t ;
	
	compute_pseudoinverse<  u2_type > compute_pinv_u2;
	compute_pinv_u2( U2_, u2_pinv_t );	
	compute_pseudoinverse<  u3_type > compute_pinv_u3;
	compute_pinv_u3( U3_, u3_pinv_t );
	
	matrix< J2, I2, T > u2_pinv = transpose( u2_pinv_t );
	matrix< J3, I3, T > u3_pinv = transpose( u3_pinv_t );
	
	//backward cyclic matricization (after Lathauwer et al., 2000a)
	tensor3< I1, J2, I3, T> tmp;
	tmp.multiply_frontal( data_, u2_pinv );
	projection_.multiply_horizontal( tmp, u3_pinv );
}
	
	
VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::optimize_mode2( const t3_type& data_, tensor3< J1, I2, J3, T >& projection_, const u1_type& U1_, const u3_type& U3_ ) const
{
	//compute pseudo inverse for matrices u2,u3
	u1_type u1_pinv_t ;
	u3_type u3_pinv_t ;
	
	compute_pseudoinverse<  u1_type > compute_pinv_u1;
	compute_pinv_u1( U1_, u1_pinv_t );	
	compute_pseudoinverse<  u3_type > compute_pinv_u3;
	compute_pinv_u3( U3_, u3_pinv_t );
	
	matrix< J1, I1, T > u1_pinv = transpose( u1_pinv_t );
	matrix< J3, I3, T > u3_pinv = transpose( u3_pinv_t );
	
	//backward cyclic matricization (after Lathauwer et al., 2000a)
	tensor3< J1, I2, I3, T> tmp;
	tmp.multiply_lateral( data_, u1_pinv );
	projection_.multiply_horizontal( tmp, u3_pinv );
}	

	
VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::optimize_mode3( const t3_type& data_, tensor3< J1, J2, I3, T >& projection_, const u1_type& U1_, const u2_type& U2_ ) const
{
	//compute pseudo inverse for matrices u2,u3
	u1_type u1_pinv_t ;
	u2_type u2_pinv_t ;
	
	compute_pseudoinverse<  u1_type > compute_pinv_u1;
	compute_pinv_u1( U1_, u1_pinv_t );
	compute_pseudoinverse<  u2_type > compute_pinv_u2;
	compute_pinv_u2( U2_, u2_pinv_t );	
	
	matrix< J1, I1, T > u1_pinv = transpose( u1_pinv_t );
	matrix< J2, I2, T > u2_pinv = transpose( u2_pinv_t );
	
	//backward cyclic matricization (after Lathauwer et al., 2000a)
	tensor3< J1, I2, I3, T> tmp;
	tmp.multiply_lateral( data_, u1_pinv );
	projection_.multiply_frontal( tmp, u2_pinv );
}
	
	
VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::hoii( const t3_type& data_ )
{
	//intialize basis matrices
	hosvd( data_ );
		
	//compute best rank-(J1, J2, J3) approximation (Lathauwer et al., 2000b)
	t3_type approximated_data;
	reconstruction( approximated_data );
	double f_norm = approximated_data.frobenius_norm();
	double max_f_norm = data_.frobenius_norm();
	//std::cout << "frobenius norm original: " << max_f_norm << std::endl;
	
	double last_f_norm = f_norm;
	double improvement = max_f_norm - f_norm;
	double min_improvement = 0.1;
	size_t i = 0;
	size_t max_iterations = 3;
	
	while( improvement > min_improvement && i < max_iterations )
	{
		
		//optimize for mode 1
		tensor3< I1, J2, J3, T > projection1; 
		optimize_mode1( data_, projection1, _u2, _u3);
		hosvd_mode1( projection1, _u1 );
		
		//optimize for mode 2
		tensor3< J1, I2, J3, T > projection2; 
		optimize_mode2( data_, projection2, _u1, _u3);
		hosvd_mode2( projection2, _u2 );
		
		//optimize for mode 3
		tensor3< J1, J2, I3, T > projection3; 
		optimize_mode3( data_, projection3, _u1, _u2);
		hosvd_mode3( projection3, _u3);
		
		set_u1( _u1 );
		set_u2( _u2 );
		set_u3( _u3 );
		derive_core_orthogonal_bases(data_, _core, _u1, _u2, _u3);
		set_core( _core );
		
		reconstruction( approximated_data );
		f_norm = approximated_data.frobenius_norm();
		improvement = f_norm - last_f_norm;
		last_f_norm = f_norm;
		
		//std::cout << "iteration '" << i << "': frobenius norm: " << std::setprecision(8) << f_norm << ", improvement: " << improvement << std::endl;
		
		++i;
	}
	
	//std::cout << "number of iterations: " << i << std::endl;
	
	derive_core_orthogonal_bases(data_, _core, _u1, _u2, _u3);
	
}	

VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::derive_core_orthogonal_bases( const t3_type& data_, t3_core_type& core_, const u1_type& U1_, const u2_type& U2_, const u3_type& U3_ )
{
	matrix< J1, I1, T > u1_inv = transpose( U1_ );
	matrix< J2, I2, T > u2_inv = transpose( U2_ );
	matrix< J3, I3, T > u3_inv = transpose( U3_ );
	
	core_.full_tensor3_matrix_multiplication( data_, u1_inv, u2_inv, u3_inv );
}
	
	
	
	
VMML_TEMPLATE_STRING
void 
VMML_TEMPLATE_CLASSNAME::derive_core( const t3_type& data_, t3_core_type& core_, const u1_type& U1_, const u2_type& U2_, const u3_type& U3_ )
{

	//compute pseudo inverse for matrices u1-u3
	u1_type u1_pinv_t ;
	u2_type u2_pinv_t ;
	u3_type u3_pinv_t ;
	
	
	compute_pseudoinverse<  u1_type > compute_pinv_u1;
	compute_pinv_u1( U1_, u1_pinv_t );
	compute_pseudoinverse<  u2_type > compute_pinv_u2;
	compute_pinv_u2( U2_, u2_pinv_t );	
	compute_pseudoinverse<  u3_type > compute_pinv_u3;
	compute_pinv_u3( U3_, u3_pinv_t );
	
	matrix< J1, I1, T > u1_pinv = transpose( u1_pinv_t );
	matrix< J2, I2, T > u2_pinv = transpose( u2_pinv_t );
	matrix< J3, I3, T > u3_pinv = transpose( u3_pinv_t );
		
	core_.full_tensor3_matrix_multiplication( data_, u1_pinv, u2_pinv, u3_pinv );


	//previous version of compute core	
	//	for( size_t j3 = 0; j3 < J3; ++j3 ) 
	//	{
	//		for( size_t j1 = 0; j1 < J1; ++j1 ) 
	//		{
	//			for( size_t j2 = 0; j2 < J2; ++j2 ) 
	//			{
	//				double sum_i1_i2_i3 = 0.0;
	//				for( size_t i3 = 0; i3 < I3; ++i3 ) 
	//				{
	//					for( size_t i1 = 0; i1 < I1; ++i1 ) 
	//					{
	//						for( size_t i2 = 0; i2 < I2; ++i2 ) 
	//						{
	//							sum_i1_i2_i3 += U1_.at( i1, j1 ) * U2_.at( i2, j2 ) * U3_.at( i3, j3 ) * data_.at( i1, i2, i3 );
	//						}
	//					}
	//				}
	//				core_.at( j1, j2, j3 ) = sum_i1_i2_i3;
	//			}
	//		}
	//	}
	//		

}
	
	



VMML_TEMPLATE_STRING
template< size_t K1, size_t K2, size_t K3>
void 
VMML_TEMPLATE_CLASSNAME::reduce_ranks( const tucker3_tensor< K1, K2, K3, I1, I2, I3, T>& other )
//TuckerJI.rank_recuction(TuckerKI) K1 -> J1, K2 -> J2, K3 -> J3; I1, I2, I3 stay the same
{
	assert(J1 <= K1);
	assert(J2 <= K2);
	assert(J3 <= K3);	
		
	//reduce basis matrices
	matrix< I1, K1, T> u1;
	other.get_u1( u1);
	for( size_t j1 = 0; j1 < J1; ++j1 ) 
	{
		_u1.set_column( j1, u1.get_column( j1 ));
	}
	
	matrix< I2, K2, T> u2;
	other.get_u2( u2 );
	for( size_t j2 = 0; j2 < J2; ++j2) 
	{
		_u2.set_column( j2, u2.get_column( j2 ));
	}
	
	matrix< I3, K3, T> u3;
	other.get_u3( u3 );
	for( size_t j3 = 0; j3 < J3; ++j3) 
	{
		_u3.set_column( j3, u3.get_column( j3 ));
	}
	
	//reduce core
	tensor3<K1, K2, K3, T> other_core;
	other.get_core( other_core );

	for( size_t j3 = 0; j3 < J3; ++j3 ) 
	{
		for( size_t j1 = 0; j1 < J1; ++j1 ) 
		{
			for( size_t j2 = 0; j2 < J2; ++j2 ) 
			{
				_core.at( j1, j2, j3 ) = other_core.at( j1, j2, j3 );
			}
		}
	}

}



VMML_TEMPLATE_STRING
template< size_t K1, size_t K2, size_t K3>
void 
VMML_TEMPLATE_CLASSNAME::subsampling( const tucker3_tensor< J1, J2, J3, K1, K2, K3, T>& other, const T& factor )
{
	assert(I1 <= K1);
	assert(I1 <= K2);
	assert(I1 <= K3);	
	
	//subsample basis matrices
	matrix< K1, J1, T> u1;
	other.get_u1( u1 );
	for( size_t i1 = 0, i = 0; i1 < K1; i1 += factor, ++i ) 
	{
		_u1.set_row( i, u1.get_row( i1 ));
	}
	
	matrix< K2, J2, T> u2;
	other.get_u2( u2 );
	for( size_t i2 = 0,  i = 0; i2 < K2; i2 += factor, ++i) 
	{
		_u2.set_row( i, u2.get_row( i2 ));
	}
	
	matrix< K3, J3, T> u3 ;
	other.get_u3( u3);
	for( size_t i3 = 0,  i = 0; i3 < K3; i3 += factor, ++i) 
	{
		_u3.set_row( i, u3.get_row( i3 ));
	}
	
	other.get_core(_core );
}


VMML_TEMPLATE_STRING
template< size_t K1, size_t K2, size_t K3>
void 
VMML_TEMPLATE_CLASSNAME::subsampling_on_average( const tucker3_tensor< J1, J2, J3, K1, K2, K3, T>& other, const T& factor )
{
	assert(I1 <= K1);
	assert(I1 <= K2);
	assert(I1 <= K3);	
	
	
	//subsample basis matrices
	matrix< K1, J1, T> u1;
	other.get_u1( u1 );
	for( size_t i1 = 0, i = 0; i1 < K1; i1 += factor, ++i )
	{
		vector< J1, T > tmp_row = u1.get_row( i1 );
		T num_items_averaged = 1;
		for( size_t j = i1+1; (j < (factor+i1)) & (j < K1); ++j, ++num_items_averaged )
			tmp_row += u1.get_row( j );

		tmp_row /= num_items_averaged;
		_u1.set_row( i, tmp_row);
	}
	
	matrix< K2, J2, T> u2;
	other.get_u2( u2 );
	for( size_t i2 = 0,  i = 0; i2 < K2; i2 += factor, ++i) 
	{
		vector< J2, T > tmp_row = u2.get_row( i2 );
		T num_items_averaged = 1;
		for( size_t j = i2+1; (j < (factor+i2)) & (j < K2); ++j, ++num_items_averaged )
			tmp_row += u2.get_row( j );

		tmp_row /= num_items_averaged;
		_u2.set_row( i, u2.get_row( i2 ));
	}
	
	matrix< K3, J3, T> u3;
	other.get_u3( u3);
	for( size_t i3 = 0,  i = 0; i3 < K3; i3 += factor, ++i) 
	{
		vector< J3, T > tmp_row = u3.get_row( i3 );
		T num_items_averaged = 1;
		for( size_t j = i3+1; (j < (factor+i3)) & (j < K3); ++j, ++num_items_averaged )
			tmp_row += u3.get_row( j );
		
		tmp_row /= num_items_averaged;
		_u3.set_row( i, u3.get_row( i3 ));
	}
  	
	 other.get_core( _core );
}




VMML_TEMPLATE_STRING
template< size_t K1, size_t K2, size_t K3>
void 
VMML_TEMPLATE_CLASSNAME::region_of_interest( const tucker3_tensor< J1, J2, J3, K1, K2, K3, T>& other, 
											const T& start_index1, const T& end_index1, 
											const T& start_index2, const T& end_index2, 
											const T& start_index3, const T& end_index3)
{
	assert(I1 <= K1);
	assert(I1 <= K2);
	assert(I1 <= K3);
	assert(start_index1 < end_index1);
	assert(start_index2 < end_index2);
	assert(start_index3 < end_index3);
	assert(end_index1 < K1);
	assert(end_index2 < K2);
	assert(end_index3 < K3);
	
	//region_of_interes of basis matrices
	matrix< K1, J1, T> u1;
	other.get_u1( u1 );
	for( size_t i1 = start_index1,  i = 0; i1 < end_index1; ++i1, ++i ) 
	{
		_u1.set_row( i, u1.get_row( i1 ));
	}
	
	matrix< K2, J2, T> u2;
	other.get_u2( u2 );
	for( size_t i2 = start_index2,  i = 0; i2 < end_index2; ++i2, ++i) 
	{
		_u2.set_row( i, u2.get_row( i2 ));
	}
	
	matrix< K3, J3, T> u3;
	other.get_u3( u3 );
	for( size_t i3 = start_index3,  i = 0; i3 < end_index3; ++i3, ++i) 
	{
		_u3.set_row( i, u3.get_row( i3 ));
	}
	
	other.get_core( _core );
	
}
	
	
VMML_TEMPLATE_STRING
void
VMML_TEMPLATE_CLASSNAME::export_to( std::vector< T >& data_ ) const
{
	u1_const_iterator  it = _u1.begin(),
	it_end = _u1.end();
	for( ; it != it_end; ++it )
	{
		data_.push_back( *it );
	}
	
	u2_const_iterator  u2_it = _u2.begin(),
	u2_it_end = _u2.end();
	for( ; u2_it != u2_it_end; ++u2_it )
	{
		data_.push_back( *u2_it );
	}

	u3_const_iterator  u3_it = _u3.begin(),
	u3_it_end = _u3.end();
	for( ; u3_it != u3_it_end; ++u3_it )
	{
		data_.push_back( *u3_it );
	}
	
	t3_core_const_iterator  it_core = _core.begin(),
	it_core_end = _core.end();
	for( ; it_core != it_core_end; ++it_core )
	{
		data_.push_back( *it_core );
	}
}
	
	
VMML_TEMPLATE_STRING
void
VMML_TEMPLATE_CLASSNAME::import_from( std::vector< T >& data_ )
{
	size_t i = 0; //iterator over data_
	
	u1_iterator  it = _u1.begin(),
	it_end = _u1.end();
	for( ; it != it_end; ++it, ++i )
	{
		*it = data_.at(i);
	}
	
	u2_iterator  u2_it = _u2.begin(),
	u2_it_end = _u2.end();
	for( ; u2_it != u2_it_end; ++u2_it, ++i )
	{
		*u2_it = data_.at(i);
	}
	
	u3_iterator  u3_it = _u3.begin(),
	u3_it_end = _u3.end();
	for( ; u3_it != u3_it_end; ++u3_it, ++i )
	{
		*u3_it = data_.at(i);
	}
	
	t3_core_iterator  it_core = _core.begin(),
	it_core_end = _core.end();
	for( ; it_core != it_core_end; ++it_core, ++i )
	{
		*it_core = data_.at(i);
	}
	
}	
	


#undef VMML_TEMPLATE_STRING
#undef VMML_TEMPLATE_CLASSNAME
	
} // namespace vmml

#endif
