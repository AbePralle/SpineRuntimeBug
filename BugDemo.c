// BugDemo.c
// https://github.com/AbePralle/SpineRuntimeBug
//
// Using spine-c runtime:
//   commit 954168892639fe208a63ebab4146ee03edea91bf (HEAD -> 4.0, origin/HEAD, origin/4.0)
//   Author: Ivan Popelyshev <ivan.popelyshev@gmail.com>
//   Date:   Thu Jul 15 07:50:04 2021 +0300
//   NBSP -> space (#1923)
#include <stdio.h>
#include "spine/spine.h"
#include "spine/extension.h"
#include "spine/SkeletonJson.h"

#define MAX_VERTICES_PER_ATTACHMENT 2048

void _spAtlasPage_createTexture( spAtlasPage* self, const char* path )
{
  // no action
}

void _spAtlasPage_disposeTexture( spAtlasPage* self )
{
  // no action
}

char* _spUtil_readFile( const char* path, int* length )
{
  printf( "Read file: %s\n", path );
  return _spReadFile( path, length );
}

void compute_drawing_coordinates( spSkeleton* skeleton );

int main()
{
  spAtlas* atlas = spAtlas_createFromFile( "Spine-T100.atlas", 0 );

  spSkeletonJson* json_reader = spSkeletonJson_create( atlas );
  spSkeletonData* skeleton_definition = spSkeletonJson_readSkeletonDataFile( json_reader, "Character.json" );
  spSkeletonJson_dispose( json_reader );

  spAnimationStateData* animation_state_definition = spAnimationStateData_create( skeleton_definition );
  animation_state_definition->defaultMix = 0.1f;

  spSkeleton* skeleton = spSkeleton_create( skeleton_definition );
  spAnimationState* animation_state = spAnimationState_create( animation_state_definition );


  spAnimationState_clearTracks( animation_state );
  spAnimationState_addAnimationByName( animation_state, 0, "attack_hook_throw", 0, 0 );
  spAnimationState_addAnimationByName( animation_state, 0, "attack_hook_pull", 0, 0 );

  for (int i=0; i<200; ++i)
  {
    printf( "%3d: ", i );
    spAnimationState_update( animation_state, 1.0/60.0 );
    spAnimationState_apply( animation_state, skeleton );
    spSkeleton_updateWorldTransform( skeleton );
    compute_drawing_coordinates( skeleton );
  }

  return 0;
}

void compute_drawing_coordinates( spSkeleton* skeleton )
{
  float v[MAX_VERTICES_PER_ATTACHMENT];

  // For each slot in the draw order array of the skeleton
  int   tracking_min_max = 0;
  float min_x, min_y, max_x, max_y;

  for (int i = 0; i < skeleton->slotsCount; ++i)
  {
    spSlot* slot = skeleton->drawOrder[i];

    spAttachment* attachment = slot->attachment;
    if (!attachment) continue;

    if (attachment->type == SP_ATTACHMENT_MESH)
    {
      spMeshAttachment* mesh = (spMeshAttachment*)attachment;

      int v_count = mesh->super.worldVerticesLength;
      if (v_count > MAX_VERTICES_PER_ATTACHMENT) continue;

      spVertexAttachment_computeWorldVertices( SUPER(mesh), slot, 0, v_count, v, 0, 2 );

      for (int i = 0; i < mesh->trianglesCount; ++i)
      {
        int index = mesh->triangles[i] << 1;
        float x = v[index];
        float y =  v[index + 1];
        if (tracking_min_max)
        {
          if (x < min_x) min_x = x;
          if (y < min_y) min_y = y;
          if (x > max_x) max_x = x;
          if (y > max_y) max_y = y;
        }
        else
        {
          tracking_min_max = 1;
          min_x = max_x = x;
          min_y = max_y = y;
        }
      }
    }
  }

  if (tracking_min_max)
  {
    float delta_x = max_x - min_x;
    float delta_y = max_y - min_y;
    float max_delta = (delta_x > delta_y) ? delta_x : delta_y;

    printf( "Max size of drawing area: %.0f\n", max_delta );
  }
}
