#include <engine/assets.hpp>

extern "C" const char* iOS_getDataPath() {
    //NSFileManager *fileManager = [NSFileManager defaultManager];
    //NSError *error;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    return [documentsDirectory UTF8String];
}
