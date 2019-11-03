#include "ModelLoadingException.h"
#include <sstream>
#include <string.h>

//
// Default constructor.
//
ModelLoadingException::ModelLoadingException() : _modelPath{ "" }
{ }

//
// Constructor that takes in an argument specifying the filename - or path of
// the model that failed to load.
//
ModelLoadingException::ModelLoadingException(const char* modelPath) : _modelPath{ modelPath }
{ }

ModelLoadingException::~ModelLoadingException() noexcept
{ }

//
// Exception details.
//
const char* ModelLoadingException::what() noexcept
{
	std::ostringstream ss;
	ss << "Model failed to load!";

	if (strlen(_modelPath) > 0)
	{
		ss << " (model path: " << _modelPath << ")";
	}

	_message = ss.str();
	return _message.c_str();
}
