#pragma once

using namespace physx;

inline glm::vec3 ptog(PxVec3 & in){
	return glm::vec3(in.x, in.y, in.z);
}

inline glm::vec3 ptog(PxExtendedVec3 & in){
	return glm::vec3(in.x, in.y, in.z);
}

inline glm::quat ptog(PxQuat & in){
	return glm::quat(in.x, in.y, in.z, in.w);
}

inline PxQuat gtop(glm::quat & in){
	return PxQuat(in.x, in.y, in.z, in.w);
}

inline PxVec3 gtop(glm::vec3 & in){
	return PxVec3(in.x, in.y, in.z);
}