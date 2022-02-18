// our compute shader yay!

struct Particles
{
    float3 pos;
    float scale;
    float3 velocity;
    float lifeSpan;
};

cbuffer ParticleConstant : register(b0)
{
    float3 velMin;
    float scaleStart;
    float3 velMax;
    float scaleRate;
    float3 startPos;
    float minTime;
    float maxTime;
    float deltaTime;
};

StructuredBuffer<Particles> inputParticle       : register(t0); // SRV
RWStructuredBuffer<Particles> outputParticle    : register(u0); // UAV

float PsuedoRandF(float min, float max, uint seedIndex, float timeOffset)
{
            // mix the seed index with a Robert Jenkins 32bit interger hash.
    seedIndex = (seedIndex + 0x7ed55d16) + (seedIndex << 12);
    seedIndex = (seedIndex ^ 0xc761c23c) ^ (seedIndex >> 19);
    seedIndex = (seedIndex + 0x165667b1) + (seedIndex << 5);
    seedIndex = (seedIndex + 0xd3a2646c) ^ (seedIndex << 9);
    seedIndex = (seedIndex + 0xfd7046c5) + (seedIndex << 3);
    seedIndex = (seedIndex ^ 0xb55a4f09) ^ (seedIndex >> 16);
            // offset our mixed seed by the timeOffset and mix one last time
    seedIndex += asuint(timeOffset);
            // second mix, once again using the Jenkins hash.
    seedIndex = (seedIndex + 0x7ed55d16) + (seedIndex << 12);
    seedIndex = (seedIndex ^ 0xc761c23c) ^ (seedIndex >> 19);
    seedIndex = (seedIndex + 0x165667b1) + (seedIndex << 5);
    seedIndex = (seedIndex + 0xd3a2646c) ^ (seedIndex << 9);
    seedIndex = (seedIndex + 0xfd7046c5) + (seedIndex << 3);
    seedIndex = (seedIndex ^ 0xb55a4f09) ^ (seedIndex >> 16);
            // with our bits avalanched sufficiently we can convert to a floating point range.
    return (seedIndex / float(0xFFFFFFFFu)) * (max - min) + min;
}


[numthreads(1, 1, 1)]
void main(uint index : SV_DispatchThreadID)
{
    
    if (inputParticle[index].lifeSpan <= 0.0f) { // respawn this particle
        outputParticle[index].lifeSpan = PsuedoRandF(minTime, maxTime, index, deltaTime);
        outputParticle[index].pos = startPos;
        outputParticle[index].scale = scaleStart;
        outputParticle[index].velocity = float3(
        cos(PsuedoRandF(velMin.x, velMax.x, index, deltaTime)),
        PsuedoRandF(velMin.y, velMax.y, index, deltaTime),
        sin(PsuedoRandF(velMin.z, velMax.z, index, deltaTime)));
        
    } else { // Update this particle
        outputParticle[index].pos = inputParticle[index].pos + inputParticle[index].velocity * deltaTime;
        outputParticle[index].scale = inputParticle[index].scale + scaleRate * deltaTime;
        outputParticle[index].lifeSpan = inputParticle[index].lifeSpan - deltaTime;
        outputParticle[index].velocity = inputParticle[index].velocity;
    }
    
}