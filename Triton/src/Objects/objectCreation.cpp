#include <Triton/Triton.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream> 

namespace Triton {
	void Luna::createAllObjects() {
		m_objects.resize(NUM_OF_OBJECTS);

		std::fstream file("Models/paths.txt");
		

		for (auto&& mesh : m_objects) {
			std::string path;
			getline(file, path);

			if (path.empty()) { throw std::runtime_error("Path of model not given"); }

			mesh.LoadModel(&m_device,path);

		}



		m_objects[0].setPosition({ 0.0,1.0,0.0 });

		file.close();
	}

	void Luna::addObject(Object& object) {
		NUM_OF_OBJECTS++;
		
		m_objects.push_back(object);
	}

}