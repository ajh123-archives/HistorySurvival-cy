#include "physic/aabb.hpp"

namespace cybrion
{
    AABB::AABB() : AABB({ 0, 0, 0 }, { 1, 1, 1 })
    {
    }

    AABB::AABB(const vec3& pos, const vec3& size) :
        m_pos(pos),
        m_size(size)
    {
    }

    vec3 AABB::getMin() const
    {
        return m_pos - (m_size / 2.0f);
    }

    vec3 AABB::getMax() const
    {
        return m_pos + (m_size / 2.0f);
    }

    vec3 AABB::getPos() const
    {
        return m_pos;
    }

    vec3 AABB::getSize() const
    {
        return m_size;
    }

    void AABB::rotate(const ivec3& rot)
    {
        mat4 rotateMat = glm::eulerAngleXYZ(
            u32(rot.x) * pi / 2,
            u32(rot.y) * pi / 2,
            u32(rot.z) * pi / 2
        );

        m_pos = rotateMat * vec4(m_pos, 1);
        
        if (rot.x % 2 != 0) std::swap(m_size.y, m_size.z);
        if (rot.y % 2 != 0) std::swap(m_size.z, m_size.x);
        if (rot.z % 2 != 0) std::swap(m_size.x, m_size.y);
    }

    AABB AABB::mergeAABB(const AABB& a, const AABB& b)
    {
        vec3 aMin = a.getMin();
        vec3 aMax = a.getMax();
        vec3 bMin = b.getMin();
        vec3 bMax = b.getMax();

        vec3 min = {
            std::min(aMin.x, bMin.x),
            std::min(aMin.y, bMin.y),
            std::min(aMin.z, bMin.z)
        };

        vec3 max = {
            std::max(aMax.x, bMax.x),
            std::max(aMax.y, bMax.y),
            std::max(aMax.z, bMax.z)
        };

        return { (min + max) / 2.0f, max - min };
    }

    // copy and modify from gist :3
    // https://gist.github.com/tesselode/e1bcf22f2c47baaedcfc472e78cac55e
    SweptAABBResult AABB::sweptAABB(const AABB& a, const AABB& b, const vec3& v)
    {
        vec3 aMin = a.getMin();
        vec3 aMax = a.getMax();

        vec3 bMin = b.getMin();
        vec3 bMax = b.getMax();

        f32 xEntry, yEntry, zEntry;
        f32 xExit, yExit, zExit;

        SweptAABBResult result{ 1, { 0, 0, 0 } };

        if (v.x == 0)
        {
            if (aMin.x < bMax.x && bMin.x < aMax.x)
            {
                xEntry = -std::numeric_limits<f32>::infinity();
                xExit = std::numeric_limits<f32>::infinity();
            }
            else
                return result;
        }
        else
        {
            f32 xEntryDis, xExitDis;

            if (v.x > 0.0f)
                xEntryDis = bMin.x - aMax.x;
            else
                xEntryDis = aMin.x - bMax.x;

            xEntry = xEntryDis / abs(v.x);

            if (v.x > 0.0f)
                xExitDis = bMax.x - aMin.x;
            else
                xExitDis = aMax.x - bMin.x;

            xExit = xExitDis / abs(v.x);
        }

        if (v.y == 0)
        {
            if (aMin.y <= bMax.y && bMin.y <= aMax.y)
            {
                yEntry = -std::numeric_limits<f32>::infinity();
                yExit = std::numeric_limits<f32>::infinity();
            }
            else
                return result;
        }
        else
        {
            f32 yEntryDis, yExitDis;

            if (v.y > 0.0f)
                yEntryDis = bMin.y - aMax.y;
            else
                yEntryDis = aMin.y - bMax.y;

            yEntry = yEntryDis / abs(v.y);

            if (v.y > 0.0f)
                yExitDis = bMax.y - aMin.y;
            else
                yExitDis = aMax.y - bMin.y;

            yExit = yExitDis / abs(v.y);
        }

        if (v.z == 0)
        {
            if (aMin.z < bMax.z && bMin.z < aMax.z)
            {
                zEntry = -std::numeric_limits<f32>::infinity();
                zExit = std::numeric_limits<f32>::infinity();
            }
            else
                return result;
        }
        else
        {
            f32 zEntryDis, zExitDis;

            if (v.z > 0.0f)
                zEntryDis = bMin.z - aMax.z;
            else
                zEntryDis = aMin.z - bMax.z;

            zEntry = zEntryDis / abs(v.z);

            if (v.z > 0.0f)
                zExitDis = bMax.z - aMin.z;
            else
                zExitDis = aMax.z - bMin.z;

            zExit = zExitDis / abs(v.z);
        }

        f32 entry = std::max({ xEntry, yEntry, zEntry });
        f32 exit = std::min({ xExit, yExit, zExit });

        if (entry < 0 || entry > 1 || entry > exit) return result;

        result.delta = entry;

        if (xEntry > zEntry)
        {
            if (xEntry > yEntry)
            {
                if (v.x < 0.0f)
                    result.normal.x = 1.0f;
                else
                    result.normal.x = -1.0f;
            }
            else
            {
                if (v.y < 0.0f)
                    result.normal.y = 1.0f;
                else
                    result.normal.y = -1.0f;
            }
        }
        else
        {
            if (zEntry > yEntry)
            {
                if (v.z < 0.0f)
                    result.normal.z = 1.0f;
                else
                    result.normal.z = -1.0f;
            }
            else
            {
                if (v.y < 0.0f)
                    result.normal.y = 1.0f;
                else
                    result.normal.y = -1.0f;
            }
        }

        return result;
    }
}
