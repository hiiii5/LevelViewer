#include "Globals.h"
#include "FloatingCameraController.h"

void FloatingCameraController::Move() {
	float spaceKeyState, leftShiftState, rightTriggerState, leftTriggerState;
	float wKeyState, sKeyState, dKeyState, aKeyState;
	float leftStickYAxisState, leftStickXAxisState;
	float rightStickYAxisState, rightStickXAxisState;
	float mouseXDelta, mouseYDelta;

	unsigned int width, height;
	Renderer::Get().GetWindowResolution(width, height);

	float deltaTime = Renderer::Get().DeltaTime;
	
	float fov = 65.0f * (3.14159f / 180.0f);

	GW::INPUT::GInput input = Renderer::Get().Input;
	GW::INPUT::GController controller = Renderer::Get().Controller;

	// Get input state changes.
	input.GetState(G_KEY_SPACE, spaceKeyState);
	input.GetState(G_KEY_LEFTSHIFT, leftShiftState);
	input.GetState(G_KEY_W, wKeyState);
	input.GetState(G_KEY_S, sKeyState);
	input.GetState(G_KEY_D, dKeyState);
	input.GetState(G_KEY_A, aKeyState);

	// If there was no change in input, then reset the deltas to 0.
	if (input.GetMouseDelta(mouseXDelta, mouseYDelta) == GW::GReturn::REDUNDANT) {
		mouseXDelta = 0.0f;
		mouseYDelta = 0.0f;
	}

	controller.GetState(0, G_RIGHT_TRIGGER_AXIS, rightTriggerState);
	controller.GetState(0, G_LEFT_TRIGGER_AXIS, leftTriggerState);
	controller.GetState(0, G_LX_AXIS, leftStickXAxisState);
	controller.GetState(0, G_LY_AXIS, leftStickYAxisState);
	controller.GetState(0, G_RX_AXIS, rightStickXAxisState);
	controller.GetState(0, G_RY_AXIS, rightStickYAxisState);

	float perFrameSpeed = 0.06f * deltaTime;

	float pitch = fov * 0.6f * mouseYDelta / height - rightStickYAxisState * perFrameSpeed * 3.0f;
	float yaw = fov * 0.6f * mouseXDelta / width + rightStickXAxisState * perFrameSpeed * 3.0f;

	// Get the change in position of the camera.
	float deltaX = dKeyState - aKeyState + leftStickXAxisState;
	float deltaY = spaceKeyState - leftShiftState + rightTriggerState - leftTriggerState;
	float deltaZ = wKeyState - sKeyState + leftStickYAxisState;

	// Scale the change by the per frame speed.
	deltaX *= perFrameSpeed;
	deltaY *= perFrameSpeed;
	deltaZ *= perFrameSpeed;

	GW::MATH::GVECTORF translation {deltaX, deltaY, deltaZ};

	// Get the matrix for the camera in world space.
	// The camera in world space is the inverse of the view matrix.
	GW::MATH::GMATRIXF camMatrix{};
	GW::MATH::GMatrix::InverseF(Renderer::Get().InstanceData.ViewMatrix, camMatrix);

	// Apply pitch and yaw changes locally to the camera.
	GW::MATH::GMatrix::RotateXLocalF(camMatrix, pitch, camMatrix);
	GW::MATH::GMatrix::RotateYGlobalF(camMatrix, yaw, camMatrix);

	// Translate the cameras matrix locally by x, y, and z.
	GW::MATH::GMatrix::TranslateLocalF(camMatrix, translation, camMatrix);

	// Move the eye position.
	GW::MATH::GVector::AddVectorF(Renderer::Get().InstanceData.EyePos, translation, Renderer::Get().InstanceData.EyePos);

	// Inverse the camera matrix to get back to the view matrix.
	GW::MATH::GMatrix::InverseF(camMatrix, Renderer::Get().InstanceData.ViewMatrix);
}
