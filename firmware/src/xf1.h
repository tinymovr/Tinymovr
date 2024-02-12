#pragma once

#include <math.h> // For fabsf() - single precision absolute value

#define DEFAULT_TRANSFORM (FrameTransformation){.offset = 0.0f, .multiplier = 1.0f}

// Structure to represent a 1D transformation from one frame to another
typedef struct {
    float offset;      // Offset to be added or subtracted
    float multiplier;  // Multiplier to scale and indicate direction
} FrameTransformation;

// Function to apply the transformation
// Applies the transformation in the form: (value * multiplier) + offset
static inline float apply_transformation(float value, const FrameTransformation* transformation) {
    return (value * transformation->multiplier) + transformation->offset;
}

// Function to apply the transformation to a velocity (ignoring offset)
static inline float apply_velocity_transformation(float velocity, const FrameTransformation* transformation) {
    return velocity * transformation->multiplier;
}

// Function to derive the inverse of a given transformation
static inline FrameTransformation derive_inverse_transformation(const FrameTransformation* original) {
    FrameTransformation inverse;
    // The inverse multiplier is the reciprocal of the original multiplier
    inverse.multiplier = 1.0f / original->multiplier;
    // To find the offset for the inverse, negate the original offset and then scale it by the inverse multiplier
    inverse.offset = -original->offset * inverse.multiplier;
    return inverse;
}

// Function to derive a FrameTransformation based on positions in two frames
static inline FrameTransformation derive_transformation(float commandedSource, float measuredSource, float commandedTarget, float measuredTarget) {
    FrameTransformation transformation;

    float sourceDelta = measuredSource - commandedSource;
    float targetDelta = measuredTarget - commandedTarget;

    if (sourceDelta == 0) {
        // If there's no change in source, the multiplier should be set to 1
        // and the offset is simply the difference between target and source positions
        transformation.multiplier = 1.0f;
        transformation.offset = commandedTarget - commandedSource;
    } else {
        // Calculate multiplier, incorporating direction based on the delta values
        transformation.multiplier = targetDelta / sourceDelta;
        // Calculate offset using one pair of points
        transformation.offset = commandedTarget - (commandedSource * transformation.multiplier);
    }

    return transformation;
}

// Function to combine two transformations
static inline FrameTransformation combine_transformations(const FrameTransformation* first, const FrameTransformation* second) {
    FrameTransformation combined;
    
    // The combined multiplier is the product of the two multipliers
    combined.multiplier = first->multiplier * second->multiplier;
    
    // The combined offset is the first offset plus the second offset scaled by the first multiplier
    combined.offset = first->offset + (second->offset * first->multiplier);
    
    return combined;
}
