#pragma once
#include <exception>
#include <string>

//
// Exception for handling models that did not load properly.
//
class ModelLoadingException : public std::exception
{
public:
	ModelLoadingException();
	ModelLoadingException(const char* modelPath);

	virtual ~ModelLoadingException() noexcept;

	virtual const char* what() noexcept;

private:
	const char* _modelPath;
	std::string _message;
};

