

struct VSIN {
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};

struct VSOUT {
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};




float4 main(VSIN IN) : SV_POSITION
{
	VSOUT OUT = (VSOUT)0;
}