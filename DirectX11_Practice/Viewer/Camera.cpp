#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: position(0, 0, 0), rotation(0, 0)
	, forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	UpdateRotation();
	UpdateView();
}

Camera::~Camera()
{

}

void Camera::GetMatrix(D3DXMATRIX * mat)
{
	memcpy(mat, &matView, sizeof(D3DXMATRIX));
}

void Camera::UpdateMove()
{
	UpdateView();
}

void Camera::UpdateRotation()
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotation.x);
	D3DXMatrixRotationY(&y, rotation.y);

	matRotation = x * y;

	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformCoord(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);
}

void Camera::UpdateView()
{
	//View Matrix ���� up�� �ַ� &D3DXVECTOR3(0, 1, 0)�� ����ȴ�. up�� �����ʿ� ����.
	//pUp [in] Pointer to the D3DXVECTOR3 structure that defines the current world's up, usually [0, 1, 0].
	/*
	zaxis = normal(At - Eye)//�ٶ󺸴� view������ upvector
	xaxis = normal(cross(Up, zaxis))//world������ up�� zaxis�� ���� = xaxis
	yaxis = cross(zaxis, xaxis)
	

	 xaxis.x           yaxis.x           zaxis.x          0
	 xaxis.y           yaxis.y           zaxis.y          0
	 xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

	*/
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &D3DXVECTOR3(0, 1, 0));
}