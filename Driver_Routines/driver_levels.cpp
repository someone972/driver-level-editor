#include "driver_levels.hpp"

DriverLevel::DriverLevel()
{
    openBlocks = 0;

    for(unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++)
        priorities[i] = DEBUG_LEVEL_NORMAL;

    log = &dummy;
};

DriverLevel::~DriverLevel()
{
    cleanup();

    //Send level destroyed event so other classes can clean up dangling pointers (don't want to unregister with an invalid level)
    eventManager.Raise(EVENT(IDriverLevelEvents::levelDestroyed)());
};

void DriverLevel::registerEventHandler(IDriverLevelEvents* handler)
{
    eventManager.Register(handler);
};

void DriverLevel::unregisterEventHandler(IDriverLevelEvents* handler)
{
    eventManager.Unregister(handler);
};

void DriverLevel::cleanup()
{
    //Send level about to be reset event.
    eventManager.Raise(EVENT(IDriverLevelEvents::levelReset)(true));

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up textures...");
    textures.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up texture definitions...");
    textureDefinitions.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up random model placements...");
    randomPlacements.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up model names...");
    modelNames.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up models...");
    models.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up event models...");
    eventModels.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up sector texture usage...");
    sectorTextures.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up visiblity tables...");
    visibility.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up world...");
    world.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up road tables...");
    roadTables.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up road connections...");
    roadConnections.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up road sections...");
    roadSections.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up intersections...");
    intersections.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up intersection positions...");
    intersectionPositions.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up heightmaps...");
    heightmaps.cleanup();
    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up heightmap tiles...");
    heightmapTiles.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up lamps...");
    lamps.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Cleaning up chairs...");
    chairs.cleanup();

    log->Log(DEBUG_LEVEL_NORMAL,"Level finished cleaning up successfully!");
    openBlocks = 0;

    //Send level has been reset event.
    eventManager.Raise(EVENT(IDriverLevelEvents::levelReset)(false));
};

void DriverLevel::setLogger(DebugLogger* newlog)
{
    if(newlog == NULL)
    log = &dummy;
    else log = newlog;
};

void DriverLevel::setLogPriorities(int levels[NUMBER_OF_BLOCKS])
{
    for(unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++)
    {
        priorities[i] = levels[i];
    }
};

int DriverLevel::loadFromFile(const char* filename, unsigned int openWhat)
{
    log->Log("Opening file %s...",filename);
    FILE* file = fopen(filename,"rb");
    if(!file)
    {
        log->Log("ERROR: Failed to open file for reading.");
        return 1;
    }
    int ret = loadFromFile(file,openWhat);
    fclose(file);
    if(ret == 2)
    log->Log("ERROR: Level is corrupt!");
    return ret;
};

int DriverLevel::loadFromFile(FILE* file,unsigned int openWhat)
{
    return load((void*)file,&fileCallbacks,openWhat);
};

int DriverLevel::load(IOHandle handle, IOCallbacks* callbacks, unsigned int openWhat)
{
    cleanup();

    int numBlocks,blockNum,blockSize;
    unsigned long start,end;
    unsigned long dataRead = 4;

    if(!handle)
    {
        log->Log("ERROR: File pointer is NULL!");
        return 1;
    }

    start = callbacks->tell(handle);
    callbacks->seek(handle,0,SEEK_END);
    end = callbacks->tell(handle);
    callbacks->seek(handle,start,SEEK_SET);

    log->Log(DEBUG_LEVEL_VERBOSE,"Blocks to load bitfield: %X",openWhat);
    log->Log(DEBUG_LEVEL_VERBOSE,"Size of handle: %d",end-start);

    callbacks->read(&numBlocks,4,1,handle);
    log->Log(DEBUG_LEVEL_VERBOSE,"Number of blocks in handle: %d",numBlocks);

    for(int i = 0; i < numBlocks; i++)
    {
        callbacks->read(&blockNum,4,1,handle);
        callbacks->read(&blockSize,4,1,handle);
        log->Log(DEBUG_LEVEL_VERBOSE,"Preparing to load block %d (%d bytes) at location 0x%X in handle.",blockNum,blockSize,callbacks->tell(handle));
        int oldPriority = log->getLogPriority();
        //check if block is corrupted or we got misaligned, if so then we bail
        if(start+blockSize+dataRead+8 > end)
        {
            if(blockNum == 14 && start+blockSize+dataRead+8 == end+4)
            {
                log->Log(DEBUG_LEVEL_IMPORTANT_ONLY,"WARNING: Block 14 (visibility block) has improper block size (developer error), shrinking by 4 bytes.");
                blockSize -= 4;
            }
            else
            {
                log->Log("ERROR: Size of block exceeds size of handle!");
                return 2;
            }
        }

        int ret = 0;

        switch(blockNum)
        {
            case BLOCK_TEXTURES:
                if(openWhat & LEV_TEXTURES)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading textures...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_TEXTURES]);
                    ret = textures.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading textures.");
                    openBlocks |= LEV_TEXTURES;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_MODELS:
                if(openWhat & LEV_MODELS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading models...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_MODELS]);
                    ret = models.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading models.");
                    openBlocks |= LEV_MODELS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_WORLD:
                if(openWhat & LEV_WORLD)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading world...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_WORLD]);
                    ret = world.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading world.");
                    openBlocks |= LEV_WORLD;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_RANDOM_MODEL_PLACEMENT:
                if(openWhat & LEV_RANDOM_MODEL_PLACEMENT)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading random model placement...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_RANDOM_MODEL_PLACEMENT]);
                    ret = randomPlacements.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading random model placement.");
                    openBlocks |= LEV_RANDOM_MODEL_PLACEMENT;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_TEXTURE_DEFINITIONS:
                if(openWhat & LEV_TEXTURE_DEFINITIONS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading texture definitions...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_TEXTURE_DEFINITIONS]);
                    ret = textureDefinitions.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading texture definitions.");
                    openBlocks |= LEV_TEXTURE_DEFINITIONS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_ROAD_TABLE:
                if(openWhat & LEV_ROAD_TABLE)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading road table...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_ROAD_TABLE]);
                    ret = roadTables.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading road table.");
                    openBlocks |= LEV_ROAD_TABLE;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_ROAD_CONNECTIONS:
                if(openWhat & LEV_ROAD_CONNECTIONS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading road connections...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_ROAD_CONNECTIONS]);
                    ret = roadConnections.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading road connections.");
                    openBlocks |= LEV_ROAD_CONNECTIONS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_INTERSECTIONS:
                if(openWhat & LEV_INTERSECTIONS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading intersections...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_INTERSECTIONS]);
                    ret = intersections.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading intersections.");
                    openBlocks |= LEV_INTERSECTIONS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_HEIGHTMAP_TILES:
                if(openWhat & LEV_HEIGHTMAP_TILES)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading heightmap tiles...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_HEIGHTMAP_TILES]);
                    ret = heightmapTiles.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading heightmap tiles.");
                    openBlocks |= LEV_HEIGHTMAP_TILES;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_HEIGHTMAP:
                if(openWhat & LEV_HEIGHTMAP)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading heightmaps...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_HEIGHTMAP]);
                    ret = heightmaps.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading heightmaps.");
                    openBlocks |= LEV_HEIGHTMAP;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_MODEL_NAMES:
                if(openWhat & LEV_MODEL_NAMES)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading model names...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_MODEL_NAMES]);
                    ret = modelNames.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading model names.");
                    openBlocks |= LEV_MODEL_NAMES;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_EVENT_MODELS:
                if(openWhat & LEV_EVENT_MODELS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading event models...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_EVENT_MODELS]);
                    ret = eventModels.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading event models.");
                    openBlocks |= LEV_EVENT_MODELS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_VISIBILITY:
                if(openWhat & LEV_VISIBILITY)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading visibility...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_VISIBILITY]);
                    ret = visibility.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading visibility.");
                    openBlocks |= LEV_VISIBILITY;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_SECTOR_TEXTURE_USAGE:
                if(openWhat & LEV_SECTOR_TEXTURE_USAGE)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading sector texture usage...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_SECTOR_TEXTURE_USAGE]);
                    ret = sectorTextures.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading sector texture usage.");
                    openBlocks |= LEV_SECTOR_TEXTURE_USAGE;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_ROAD_SECTIONS:
                if(openWhat & LEV_ROAD_SECTIONS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading road sections...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_ROAD_SECTIONS]);
                    ret = roadSections.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading road sections.");
                    openBlocks |= LEV_ROAD_SECTIONS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_INTERSECTION_POSITIONS:
                if(openWhat & LEV_INTERSECTION_POSITIONS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading intersection positions...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_INTERSECTION_POSITIONS]);
                    ret = intersectionPositions.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading intersection positions.");
                    openBlocks |= LEV_INTERSECTION_POSITIONS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_LAMPS:
                if(openWhat & LEV_LAMPS)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading lamps...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_LAMPS]);
                    ret = lamps.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading lamps.");
                    openBlocks |= LEV_LAMPS;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            case BLOCK_CHAIR_PLACEMENT:
                if(openWhat & LEV_CHAIR_PLACEMENT)
                {
                    log->Log(DEBUG_LEVEL_NORMAL,"Loading chair placement...");
                    log->increaseIndent();
                    log->setLogPriority(priorities[BLOCK_CHAIR_PLACEMENT]);
                    ret = chairs.load(handle, callbacks, blockSize, log);
                    log->setLogPriority(oldPriority);
                    log->decreaseIndent();
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished loading chair placement.");
                    openBlocks |= LEV_CHAIR_PLACEMENT;
                }
                else callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
            default:
                log->Log(DEBUG_LEVEL_NORMAL,"Unknown/Unused block type %d of size %d encountered. Skipped.", blockNum, blockSize);
                callbacks->seek(handle,blockSize,SEEK_CUR);
                break;
        }
        dataRead += 8+blockSize;
        if(ret != 0)
        {
            log->Log("Block load failed. Aborting level loading!");
            cleanup();
            return 1;
        }
    }
    log->Log(DEBUG_LEVEL_NORMAL,"Level finished loading successfully!");

    //Send level opened event.
    eventManager.Raise(EVENT(IDriverLevelEvents::levelOpened)());
    return 0;
};

int DriverLevel::saveToFile(const char* filename, unsigned int saveWhat)
{
    log->Log("Saving to file %s...",filename);
    FILE* file = fopen(filename,"wb");
    if(!file)
    {
        log->Log("ERROR: Failed to open file for writing.");
        return 1;
    }
    int ret = saveToFile(file,saveWhat);
    fclose(file);
    return ret;
};

int DriverLevel::saveToFile(FILE* file,unsigned int saveWhat)
{
    return save((void*)file,&fileCallbacks,saveWhat);
};

int DriverLevel::save(IOHandle handle, IOCallbacks* callbacks, unsigned int saveWhat)
{
    int numBlocks = 0;
    int blockNum;
    unsigned int blockSize;

    const unsigned int defaultOrderBlocks[NUMBER_OF_BLOCKS] = {BLOCK_MODEL_NAMES,BLOCK_TEXTURES,BLOCK_MODELS,BLOCK_WORLD,3,BLOCK_RANDOM_MODEL_PLACEMENT,BLOCK_TEXTURE_DEFINITIONS,
                                                               6,BLOCK_ROAD_TABLE,BLOCK_ROAD_CONNECTIONS,BLOCK_INTERSECTIONS,BLOCK_HEIGHTMAP_TILES,BLOCK_HEIGHTMAP,BLOCK_ROAD_SECTIONS,
                                                               BLOCK_INTERSECTION_POSITIONS,BLOCK_EVENT_MODELS,BLOCK_SECTOR_TEXTURE_USAGE,BLOCK_LAMPS,BLOCK_CHAIR_PLACEMENT,18,BLOCK_VISIBILITY};
    const unsigned int defaultOrderBits[NUMBER_OF_BLOCKS] = {LEV_MODEL_NAMES,LEV_TEXTURES,LEV_MODELS,LEV_WORLD,0,LEV_RANDOM_MODEL_PLACEMENT,LEV_TEXTURE_DEFINITIONS,
                                                             0,LEV_ROAD_TABLE,LEV_ROAD_CONNECTIONS,LEV_INTERSECTIONS,LEV_HEIGHTMAP_TILES,LEV_HEIGHTMAP,LEV_ROAD_SECTIONS,
                                                             LEV_INTERSECTION_POSITIONS,LEV_EVENT_MODELS,LEV_SECTOR_TEXTURE_USAGE,LEV_LAMPS,LEV_CHAIR_PLACEMENT,0,LEV_VISIBILITY};


    //Send level about to be saved event.
    eventManager.Raise(EVENT(IDriverLevelEvents::levelSaved)(true));

    if(!handle)
    {
        log->Log("ERROR: File pointer is NULL!");
        return 1;
    }

    log->Log(DEBUG_LEVEL_VERBOSE,"Blocks to save bitfield: %X",saveWhat);

    for(unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++)
    {
        if(saveWhat & defaultOrderBits[i])
        numBlocks++;
    }
    log->Log(DEBUG_LEVEL_VERBOSE,"Number of blocks to write: %d",numBlocks);
    callbacks->write(&numBlocks,4,1,handle);

    for(unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++)
    {
        if(saveWhat & defaultOrderBits[i])
        {
            blockNum = defaultOrderBlocks[i];
            log->Log(DEBUG_LEVEL_VERBOSE,"Preparing to write block %d at location 0x%X in handle.",blockNum,callbacks->tell(handle));
            callbacks->write(&blockNum,4,1,handle);

            switch(blockNum)
            {
                case BLOCK_TEXTURES:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving textures...");
                    blockSize = textures.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    textures.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving textures.");
                    break;
                case BLOCK_MODELS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving models...");
                    blockSize = models.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    models.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving models.");
                    break;
                case BLOCK_WORLD:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving world...");
                    blockSize = world.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    world.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving world.");
                    break;
                case BLOCK_RANDOM_MODEL_PLACEMENT:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving random model placement...");
                    blockSize = randomPlacements.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    randomPlacements.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving random model placement.");
                    break;
                case BLOCK_TEXTURE_DEFINITIONS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving texture definitions...");
                    blockSize = textureDefinitions.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    textureDefinitions.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving texture definitions.");
                    break;
                case BLOCK_ROAD_TABLE:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving road table...");
                    blockSize = roadTables.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    roadTables.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving road table.");
                    break;
                case BLOCK_ROAD_CONNECTIONS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving road connections...");
                    blockSize = roadConnections.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    roadConnections.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving road connections.");
                    break;
                case BLOCK_INTERSECTIONS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving intersections...");
                    blockSize = intersections.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    intersections.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving intersections.");
                    break;
                case BLOCK_HEIGHTMAP_TILES:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving heightmap tiles...");
                    blockSize = heightmapTiles.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    heightmapTiles.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving heightmap tiles.");
                    break;
                case BLOCK_HEIGHTMAP:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving heightmaps...");
                    blockSize = heightmaps.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    heightmaps.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving heightmaps.");
                    break;
                case BLOCK_MODEL_NAMES:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving model names...");
                    blockSize = modelNames.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    modelNames.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving model names.");
                    break;
                case BLOCK_EVENT_MODELS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving event models...");
                    blockSize = eventModels.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    eventModels.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving event models.");
                    break;
                case BLOCK_VISIBILITY:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving visibility...");
                    blockSize = visibility.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    visibility.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving visibility.");
                    break;
                case BLOCK_SECTOR_TEXTURE_USAGE:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving sector texture usage...");
                    blockSize = sectorTextures.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    sectorTextures.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving sector texture usage.");
                    break;
                case BLOCK_ROAD_SECTIONS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving road sections...");
                    blockSize = roadSections.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    roadSections.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving road sections.");
                    break;
                case BLOCK_INTERSECTION_POSITIONS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving intersection positions...");
                    blockSize = intersectionPositions.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    intersectionPositions.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving intersection positions.");
                    break;
                case BLOCK_LAMPS:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving lamps...");
                    blockSize = lamps.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    lamps.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving lamps.");
                    break;
                case BLOCK_CHAIR_PLACEMENT:
                    log->Log(DEBUG_LEVEL_NORMAL,"Saving chair placement...");
                    blockSize = chairs.getRequiredSize();
                    log->Log(DEBUG_LEVEL_DEBUG,"Block size should be %d bytes.",blockSize);
                    callbacks->write(&blockSize,4,1,handle);
                    chairs.save(handle, callbacks);
                    log->Log(DEBUG_LEVEL_NORMAL,"Finished saving chair placement.");
                    break;
            }
        }
    }
    log->Log(DEBUG_LEVEL_NORMAL,"Level finished saving successfully!");

    //Send level has been saved event.
    eventManager.Raise(EVENT(IDriverLevelEvents::levelSaved)(false));
    return 0;
};
