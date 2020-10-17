#include "telrad.h"

Telrad::Telrad()
{

}

void Telrad::TelradShow()
{
StelCore *core = StelApp::getInstance().getCore();
const StelProjectorP projector = core->getProjection(StelCore::FrameEquinoxEqu);
// StelPainter drawing
StelPainter painter(projector);

Vec3f lineColor=Vec3f(0.77,0.14,0.16);

painter.setColor(0.77,0.14,0.16,1);

Vec2i centerScreen(projector->getViewportPosX()+projector->getViewportWidth()/2,
           projector->getViewportPosY()+projector->getViewportHeight()/2);
const float pixelsPerRad = projector->getPixelPerRadAtCenter(); // * params.devicePixelsPerPixel;
painter.drawCircle(centerScreen[0], centerScreen[1], 0.5f * pixelsPerRad * static_cast<float>(M_PI/180) * (0.5));
painter.drawCircle(centerScreen[0], centerScreen[1], 0.5f * pixelsPerRad * static_cast<float>(M_PI/180) * (1));
painter.drawCircle(centerScreen[0], centerScreen[1], 0.5f * pixelsPerRad * static_cast<float>(M_PI/180) * (2));
painter.drawCircle(centerScreen[0], centerScreen[1], 0.5f * pixelsPerRad * static_cast<float>(M_PI/180) * (4));

    qDebug()<<"centerScreen:"<<centerScreen[0]<< centerScreen[1];
    qDebug()<<"Circle:"<<0.5f * pixelsPerRad * static_cast<float>(M_PI/180) * (0.5);

}
