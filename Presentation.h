#pragma once
#include "SceneObject.h"
#include "TextShape.h"
#include "Mesh.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "AmbientLight.h"
#include "SpotLight.h"

//
// Starts a presentation to showcase all features of the framework.
//
class Presentation : public SceneObject
{
	//
	// Current state of the presentation.
	//
	enum class PresentationStage
	{
		PHASE_BEGIN,
		PHASE_WIREFRAME,
		PHASE_MARVIN,
		PHASE_BOUNCE,
		PHASE_BACKFACE,
		PHASE_POLYGON,
		PHASE_DIRECTIONAL,
		PHASE_FLAT,
		PHASE_VERTEX,
		PHASE_FRAGMENT,
		PHASE_POINT,
		PHASE_SPOT,
		PHASE_END
	};

public:
	//
	// Set initial conditions/background colour.
	//
	void OnInit() override;

	//
	// Runs the presentation cycle.
	//
	void OnTick(const float& deltaTime) override;

private:
	//
	// Phase callbacks.
	//
	void BeginPhase(const float& deltaTime);
	void WireframePhase(const float& deltaTime);
	void MarvinPhase(const float& deltaTime);
	void BouncePhase(const float& deltaTime);
	void BackfacePhase(const float& deltaTime);
	void PolygonPhase(const float& deltaTime);
	void DirectionalPhase(const float& deltaTime);
	void FlatPhase(const float& deltaTime);
	void VertexPhase(const float& deltaTime);
	void FragmentPhase(const float& deltaTime);
	void PointPhase(const float& deltaTime);
	void SpotPhase(const float& deltaTime);
	void EndPhase(const float& deltaTime);

private:
	TextShape* _displayText{ nullptr };
	PresentationStage _state;

	// Scene objects
	Mesh* _pedistal{ nullptr };
	Mesh* _figurine{ nullptr };

	AmbientLight* _ambient{ nullptr };
	DirectionalLight* _directional{ nullptr };
	PointLight* _point{ nullptr };
	SpotLight* _spot{ nullptr };

	float _delay{ 0 };
	bool _doRotateFigurine{ false };

	// Phase data
	float _cubeRotationValue{ 0 };

	bool _preSquish{ true };
	bool _preBounce{ true };

	float _lightsDelay{ 1.f };
};