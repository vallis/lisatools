%include typemaps.i

%{
#include "numpy/arrayobject.h"

#define ISCONTIGUOUS(m) (PyArray_ISCONTIGUOUS(m))

#define PyArray_CONTIGUOUS(m) (ISCONTIGUOUS(m) ? Py_INCREF(m), m : \
(PyArrayObject *)(PyArray_ContiguousFromObject((PyObject *)(m), (m)->descr->type_num, 0,0)))
%}

%init %{
  import_array();
%}

// No checking of array length (hope for the best)

%typemap(in) double *carray {
	PyArrayObject *arr;
	
	if (!PyArray_Check($input)) {
		PyErr_SetString(PyExc_TypeError,"First argument is not an array");
		return NULL;
	}
	
	if (PyArray_ObjectType($input,0) != PyArray_DOUBLE) {
		PyErr_SetString(PyExc_TypeError, "Incorrect array type: we need an array of DOUBLE");
		return NULL;
	}
	
	arr = PyArray_CONTIGUOUS((PyArrayObject *)$input);

	if (arr->nd == 1 || arr->nd == 2) {
		$1 = (double *)arr->data;
	} else {
		PyErr_SetString(PyExc_TypeError, "Incorrect number of dims: we want a 1D or 2D array");
		return NULL;
	}
	
	Py_DECREF(arr);  /* Release our local copy of the PyArray */
}


%typemap(in) (double *numpyarray,long numpyarraysize) {
	PyArrayObject *arr;
	
	if (!PyArray_Check($input)) {
		PyErr_SetString(PyExc_TypeError,"First argument is not an array");
		return NULL;
	}
	
	if (PyArray_ObjectType($input,0) != PyArray_DOUBLE) {
		PyErr_SetString(PyExc_TypeError, "Incorrect array type: we need an array of DOUBLE");
		return NULL;
	}
	
	arr = PyArray_CONTIGUOUS((PyArrayObject *)$input);

	if (arr->nd == 1) {
		$1 = (double *)arr->data;
		$2 = (long)arr->dimensions[0];
	} else if (arr->nd == 2) {
		$1 = (double *)arr->data;
		$2 = (long)arr->dimensions[0] * (long)arr->dimensions[1];
	} else {
		PyErr_SetString(PyExc_TypeError, "Incorrect number of dims: we want a 1D or 2D array");
		return NULL;
	}
	
	Py_DECREF(arr);  /* Release our local copy of the PyArray */
}
