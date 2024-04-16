float4 main(uint2 pos : Position): SV_Position
{
    return float4( pos.x/(64.0f / 2) - 1, 1 - pos.y/(32.0f/2) , 0, 1.0f);
}