// our compute shader yay!

struct Particles
{
    float3 pos;
    float scale;
    float3 velocity;
    float lifeSpan;
};

StructuredBuffer<Particles> inputParticle       : register(t0); // SRV
RWStructuredBuffer<Particles> outputParticle    : register(u0); // UAV

[numthreads(1, 1, 1)]
void main(uint index : SV_DispatchThreadID)
{
    // pretty simple update for now just to test set up
    outputParticle[index].pos = inputParticle[index].pos; 
    outputParticle[index].scale = inputParticle[index].scale + 0.001f;
    outputParticle[index].velocity = inputParticle[index].velocity;
    outputParticle[index].lifeSpan = inputParticle[index].lifeSpan + 1.0f;
}