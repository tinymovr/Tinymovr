#pragma once

#include <math.h> // For fabsf() - single precision absolute value

#define DEFAULT_TRANSFORM (FrameTransform){.offset = 0.0f, .multiplier = 1.0f}

// Structure to represent a 1D transformation from one frame to another
typedef struct {
    float offset;      // Offset to be added or subtracted
    float multiplier;  // Multiplier to scale and indicate direction
} FrameTransform;

// Function to apply the transformation
// Applies the transformation in the form: (value * multiplier) + offset
static inline float apply_transform(float value, const FrameTransform* transformation) {
    return (value * transformation->multiplier) + transformation->offset;
}

// Function to apply the transformation to a velocity (ignoring offset)
static inline float apply_velocity_transform(float velocity, const FrameTransform* transformation) {
    return velocity * transformation->multiplier;
}

// Function to derive the inverse of a given transformation
static inline FrameTransform derive_inverse_transform(const FrameTransform* original) {
    FrameTransform inverse;
    // The inverse multiplier is the reciprocal of the original multiplier
    inverse.multiplier = 1.0f / original->multiplier;
    // To find the offset for the inverse, negate the original offset and then scale it by the inverse multiplier
    inverse.offset = -original->offset * inverse.multiplier;
    return inverse;
}

// Function to derive a FrameTransform based on positions in two frames
static inline FrameTransform derive_transform_2p(float As, float At, float Bs, float Bt) {
    FrameTransform transformation;

    float sourceDelta = Bs - As;
    float targetDelta = Bt - At;

    if (sourceDelta == 0) {
        // If there's no change in source, the multiplier should be set to 1
        // and the offset is simply the difference between target and source positions
        transformation.multiplier = 1.0f;
        transformation.offset = At - As;
    } else {
        // Calculate multiplier, incorporating direction based on the delta values
        transformation.multiplier = targetDelta / sourceDelta;
        // Calculate offset using one pair of points
        transformation.offset = At - (As * transformation.multiplier);
    }

    return transformation;
}

static inline FrameTransform derive_dir_transform_2p(float As, float At, float Bs, float Bt) {
    FrameTransform transform;

    // Calculate the direction of the movement in both frames
    float sourceDirection = Bs - As;
    float targetDirection = Bt - At;

    // Determine the multiplier based on the direction comparison
    transform.multiplier = (sourceDirection * targetDirection > 0) ? 1.0f : -1.0f;

    // Use the first pair of points and the determined multiplier to calculate the offset
    // If the multiplier is 1.0, offset = At - As; if -1.0, offset = At + As
    transform.offset = At - (As * transform.multiplier);

    return transform;
}

// Function to combine two transformations
static inline FrameTransform combine_transforms(const FrameTransform* first, const FrameTransform* second) {
    FrameTransform combined;
    
    // The combined multiplier is the product of the two multipliers
    combined.multiplier = first->multiplier * second->multiplier;
    
    // The combined offset is the first offset plus the second offset scaled by the first multiplier
    combined.offset = first->offset + (second->offset * first->multiplier);
    
    return combined;
}
