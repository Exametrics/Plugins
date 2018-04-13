#ifndef Q_EXAMETRICS_UTILS_H
#define Q_EXAMETRICS_UTILS_H
#define CC_LAS_SUPPORT true

#include <math.h>
#include <CCGeom.h> // ccvector3
#include <QString>
#include <QList>
#include <QPair>
#include "LASFilter.h"
#include "FileIOFilter.h"

class Utils
{
    public:

        static void saveCloudToLasFile(ccGenericPointCloud* cloud, QString fileName, QWidget* parentWidget);

        static bool double_equals(double a, double b, double epsilon = 0.01);

        static double frand_a_b(double a, double b);

        static QString ccVector3ToString(const CCVector3 v);
        static QString ccVector3ToString(const CCVector3d v);
        static QString ccVector3ToString(const CCVector3* v);
        /*static QString ccPVector3ToString(CCVector3* v);
        static QString ccPVector3ToString(CCVector3d* v);*/

        static CCVector3 ccVectorDoubleToFloat(CCVector3d v);
        //static CCVector3* ccPVectorDoubleToFloat(CCVector3d* v);

        static CCVector3 ccVectorDoublePToFloat(CCVector3d* v);


        struct QPairSecondComparer
        {
            template<typename T1, typename T2>
            bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
            {
                return a.second < b.second;
            }
        };
};


#endif
