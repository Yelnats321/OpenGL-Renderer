#pragma once

using namespace physx;

inline glm::vec3 ptog(PxVec3 & in){
	return glm::vec3(in.x, in.y, in.z);
}

inline glm::vec3 ptog(PxExtendedVec3 & in){
	return glm::vec3(in.x, in.y, in.z);
}

//due to something retarded, physx has w-x-y-z and gml has x-y-z-w 
inline glm::quat ptog(PxQuat & in){
	return glm::quat(in.w, in.x, in.y, in.z);
}

//this rotation change might be wrong!!!!!
/*inline PxQuat gtop(glm::quat & in){
	return PxQuat(in.z, in.y, in.x, in.w);
}*/

inline PxVec3 gtop(glm::vec3 & in){
	return PxVec3(in.x, in.y, in.z);
}