/**
 * @file PyLoadableBehavior.cpp
 *  @author Arne Boeckmann (arne.boeckmann@dfki.de)
 */

#include <Python.h>
#include "PyLoadableBehavior.h"
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream> //FIXME remove after testing
#include <sstream>
using namespace std;

namespace behavior_learning { namespace bl_loader {

PyLoadableBehavior::PyLoadableBehavior(lib_manager::LibManager *libManager,
                                       const std::string &libName,
                                       const int libVersion) :
                                       LoadableBehavior(libManager, libName, libVersion,
                                                        0, 0) //numInputs and numOutputs will be set in initialize()
{}


bool PyLoadableBehavior::initialize(const std::string& initialConfigPath)
{
  //this will throw a runtime_error if instantiation failed
  py_behavior = Helper::instance().getClassInstance("behavior", initialConfigPath);

  pyBehavior.reset(PyBehavior::fromPyObject(py_behavior.get()));
  setNumInputs(pyBehavior->getNumInputs());
  setNumOutputs(pyBehavior->getNumOutputs());
  return true;
}

bool PyLoadableBehavior::configure(const std::string& configPath)
{
  std::ifstream fin(configPath.c_str());
  YAML::Parser parser(fin);
  return configureFromYamlParser(parser);
}

bool PyLoadableBehavior::configureYaml(const string& yaml)
{
  std::stringstream sin(yaml);
  YAML::Parser parser(sin);
  return configureFromYamlParser(parser);
}

bool PyLoadableBehavior::configureFromYamlParser(YAML::Parser& parser)
{
  /**
   * Parse the yaml file into PyBehavior::MetaParameters
   * and call pyBehavior->setMetaParameters
   */
  YAML::Node doc;
    if(parser.GetNextDocument(doc)) //we assume that there is only one document
    {
      PyBehavior::MetaParameters parameters;
      for(YAML::Iterator it = doc.begin(); it != doc.end(); ++it)
      {
          string key;
          it.first() >> key;
          parameters[key] = vector<double>();
          it.second() >> parameters[key];
      }
      pyBehavior->setMetaParameters(parameters);
      return true;
    }
    else
    {
      //No document in yaml file
      std::cerr << "Invalid yaml document" << endl;
      return false;
    }
}

void PyLoadableBehavior::setInputs(const double* values, int numInputs)
{
  pyBehavior->setInputs(values, numInputs);
}

void PyLoadableBehavior::getOutputs(double* values, int numOutputs) const
{
  pyBehavior->getOutputs(values, numOutputs);
}

void PyLoadableBehavior::step()
{
  pyBehavior->step();
}

bool PyLoadableBehavior::canStep() const
{
  return pyBehavior->canStep();
}

}}//end namespace