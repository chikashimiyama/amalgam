//
//  SceneComponent.h
//  amalgam
//
//  Created by Chikashi Miyama on 30/07/14.
//
//

#ifndef __amalgam__SceneComponent__
#define __amalgam__SceneComponent__

class SceneComponent{
protected:
    bool drawFlag;
public:
    void toggleDrawFlag();
    
};

inline void SceneComponent::toggleDrawFlag(){
    drawFlag = !drawFlag;
}

#endif /* defined(__amalgam__File__) */
