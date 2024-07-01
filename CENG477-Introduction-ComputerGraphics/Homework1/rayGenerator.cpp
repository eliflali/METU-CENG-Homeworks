#include "parser.h"
#include "mathFunctions.h"
#include "rayGenerator.h"

using namespace parser;
using namespace rayGenerator;
using namespace mathFunctions;

typedef unsigned char RGB[3];


namespace rayGenerator
{
    Ray generateRay(const Camera &camera, int i, int j)
    {
        float su = (camera.near_plane.y - camera.near_plane.x) * (j + 0.5) / camera.image_width;
        float sv = (camera.near_plane.w - camera.near_plane.z) * (i + 0.5) / camera.image_height;

        Vec3f gaze = normalize(camera.gaze);
        Vec3f u = normalize(crossProduct(gaze, camera.up));
        Vec3f v = crossProduct(u, gaze);

        Vec3f m = computeM(camera);
        Vec3f q = computeQ(m, u, v, camera);
        Vec3f s = computeS(q, u, v, su, sv);

        Ray ray;
        ray.origin = camera.position;
        ray.direction = normalize(subtractVectors(s, camera.position));

        return ray;
    }

    Vec3f computeM(const Camera &camera)
    {
        Vec3f M;
        Vec3f gaze = normalize(camera.gaze);
        M.x = camera.position.x + (gaze.x * camera.near_distance);
        M.y = camera.position.y + (gaze.y * camera.near_distance);
        M.z = camera.position.z + (gaze.z * camera.near_distance);
        return M;
    }

    Vec3f computeQ(const Vec3f &m, const Vec3f &u, const Vec3f &v, const Camera &camera)
    {
        Vec3f Q;
        Q.x = m.x + (u.x * camera.near_plane.x) + (v.x * camera.near_plane.w);
        Q.y = m.y + (u.y * camera.near_plane.x) + (v.y * camera.near_plane.w);
        Q.z = m.z + (u.z * camera.near_plane.x) + (v.z * camera.near_plane.w);
        return Q;
    }

    Vec3f computeS(const Vec3f &q, const Vec3f &u, const Vec3f &v, float su, float sv)
    {
        Vec3f S;
        S.x = q.x + (u.x * su) - (v.x * sv);
        S.y = q.y + (u.y * su) - (v.y * sv);
        S.z = q.z + (u.z * su) - (v.z * sv);
        return S;
    }
}


