///////////////////////////////////////////////////////////////////////////////////////////////////
<<<<<<< HEAD
// OpenGL Mathematics Copyright (c) 2005 - 2013 G-Truc Creation (www.g-truc.net)
=======
// OpenGL Mathematics Copyright (c) 2005 - 2012 G-Truc Creation (www.g-truc.net)
>>>>>>> Adding-math
///////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2005-12-21
// Updated : 2009-03-06
// Licence : This source is under MIT License
// File    : glm/gtx/perpendicular.inl
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace glm
{
	template <typename vecType> 
	GLM_FUNC_QUALIFIER vecType perp
	(
		vecType const & x, 
		vecType const & Normal
	)
	{
		return x - proj(x, Normal);
	}
}//namespace glm
