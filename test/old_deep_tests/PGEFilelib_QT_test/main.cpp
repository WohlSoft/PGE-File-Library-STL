
#include <QCoreApplication>
#include <QTextStream>
#include <file_formats.h>

//for DEEP test of SMBX38A
#include <QDir>
#include <QElapsedTimer>

void printLevelInfo(LevelData &lvl, QTextStream &cout)
{
    cout<<"Level title: "<< lvl.LevelName << "\n";
    cout<<"Num of stars: "<< lvl.stars << "\n";
    cout<<"Sections: "<< lvl.sections.size() << "\n";
    cout<<"Blocks: "<< lvl.blocks.size() << "\n";
    cout<<"BGO's: "<< lvl.bgo.size() << "\n";
    cout<<"NPC's: "<< lvl.npc.size() << "\n";
    cout<<"Warps: "<< lvl.doors.size() << "\n";
    cout<<"Physical EnvZones: "<< lvl.physez.size() << "\n";
    cout<<"Layers: "<< lvl.layers.size() << "\n";
    cout<<"Events: "<< lvl.events.size() << "\n";
    cout<<"Variables: "<< lvl.variables.size() << "\n";
    if(lvl.scripts.size()>0)
    {
        cout<< "Test of script printing" << "\n";
        for(int i=0; i<lvl.scripts.size(); i++)
        {
            cout<< "===========" << lvl.scripts[i].name << "============" << "\n";
            cout<<lvl.scripts[i].script << "\n";
        }
        cout<< "==================================" << "\n";
    }
    cout.flush();
}

void printWorldInfo(WorldData &lvl, QTextStream &cout)
{
    cout<<"Episode title: "<< lvl.EpisodeTitle<< "\n";
    cout<<"Num of stars: "<< lvl.stars << "\n";
    cout<<"Credits:\n"<< lvl.authors << "\n\n";
    cout<<"Tiles: "<< lvl.tiles.size() << "\n";
    cout<<"Sceneries: "<< lvl.scenery.size() << "\n";
    cout<<"Paths: "<< lvl.paths.size() << "\n";
    cout<<"Levels: "<< lvl.levels.size() << "\n";
    cout<<"Musicboxes: "<< lvl.music.size() << "\n\n";
    cout.flush();
}

void printLine(QTextStream &cout)
{
    cout <<"\n\n=============================\n";
    cout.flush();
}

QString flString(QString str, int lenght)
{
    QString fn;
    fn.resize(lenght);
    fn.fill(' ', lenght);
    for(int i=0;(i<str.size()) && (i<str.size()); i++)
        fn[i]=str[i];
    return fn;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream cout(stdout);
    char endl='\n';

    LevelData level;
    printLine(cout);
    cout << "\n\nSMBX64 Level Read Header test:" << endl;
    FileFormats::ReadSMBX64LvlFileHeader("test.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
    }
    printLine(cout);

    cout << "\n\nSMBX64 Level Read test:" << endl;
    FileFormats::OpenLevelFile("test.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
        FileFormats::SaveLevelFile(level, "test_out_64.lvl", FileFormats::LVL_SMBX64, 64);
        FileFormats::SaveLevelFile(level, "test_out_50.lvl", FileFormats::LVL_SMBX64, 50);
        FileFormats::SaveLevelFile(level, "test_out_01.lvl", FileFormats::LVL_SMBX64, 1);
    }

    #define BGO_ORDER_RESEARCH
    #ifdef BGO_ORDER_RESEARCH
    //Make a dummy level for BGO Order priority grabbing test
    {
        LevelData bgo_test = FileFormats::CreateLevelData();
        bgo_test.sections[0].size_left=0;
        bgo_test.sections[0].size_top=0;
        bgo_test.sections[0].size_right=800;
        bgo_test.sections[0].size_bottom=600;
        for(int i=1; i<=190; i++)
        {
            LevelBGO bgo = FileFormats::CreateLvlBgo();
            bgo.id = static_cast<unsigned long>(i);
            bgo_test.bgo.push_back(bgo);
        }
        FileFormats::SaveLevelFile(bgo_test, "test_BGO_Test_out.lvl", FileFormats::LVL_SMBX64, 64);
    }
    //take set of different files and build a table of BGOs
    {
        #define NumOfFiles 12
        LevelData tonnOfBGOs[NumOfFiles];
        const char *fileNames[NumOfFiles] = {
            "test_BGO_Test_noBlocks.lvl",
            "test_BGO_Test_SzBlocks1.lvl",
            "test_BGO_Test_SzBlocks2.lvl",
            "test_BGO_Test_SzBlocks3.lvl",

            "test_BGO_Test_SzBlocks4.lvl",
            "test_BGO_Test_SzBlocks5.lvl",
            "test_BGO_Test_SzBlocks6.lvl",
            "test_BGO_Test_SzBlocks7.lvl",

            "test_BGO_Test_SzBlocks8.lvl",
            "test_BGO_Test_SzBlocks9.lvl",
            "test_BGO_Test_SzBlocks0.lvl",
            "test_BGO_Test_SzBlocksX.lvl"
        };
        for(int i=0; i<NumOfFiles; i++)
        {
            FileFormats::OpenLevelFile(fileNames[i], tonnOfBGOs[i]);
        }
        QFile listOfBGOs("bgos_table_out.csv");
        listOfBGOs.open(QIODevice::WriteOnly|QIODevice::Text);
        QTextStream lobout(&listOfBGOs);
        for(int i=0;i<190; i++)
        {
            for(int j=0;j<NumOfFiles; j++)
            {
                lobout << tonnOfBGOs[j].bgo[i].id << (j<(NumOfFiles-1) ? "," : "");
            }
            lobout << "\n";
        }
        listOfBGOs.close();

    }
    #endif


    printLine(cout);
    cout << "\n\nSMBX65-38A Level Read Header test:" << endl;
    FileFormats::ReadSMBX38ALvlFileHeader("test_65-38a.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nSMBX65-38A Level Read test #1:" << endl;
    FileFormats::OpenLevelFile("test_65-38a.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
        FileFormats::WriteExtendedLvlFileF("test_65-38a-out.lvlx", level);
    }

    printLine(cout);
    cout << "\n\nSMBX65-38A Level Read test #2:" << endl;
    FileFormats::OpenLevelFile("test-38a-2.lvl", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
        FileFormats::WriteExtendedLvlFileF("test_65-38a-2-out.lvlx", level);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read Header test:" << endl;
    FileFormats::ReadExtendedLvlFileHeader("test.lvlx", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read test №2:" << endl;
    FileFormats::OpenLevelFile("test2.lvlx", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X Level Read test №1:" << endl;
    FileFormats::OpenLevelFile("test.lvlx", level);
    cout << level.meta.filename << "\n";
    cout << level.meta.path << "\n";
    if(!level.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << level.meta.ERROR_info;
    } else {
        printLevelInfo(level,cout);
    }

    WorldData world;
    printLine(cout);
    cout << "\n\nSMBX64 World Read Header test:" << endl;
    FileFormats::ReadSMBX64WldFileHeader("test.wld", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << world.meta.ERROR_info;
    } else {
        printWorldInfo(world,cout);
    }

    printLine(cout);
    cout << "\n\nSMBX64 World Read test:" << endl;
    FileFormats::OpenWorldFile("test.wld", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << world.meta.ERROR_info;
    } else {
        printWorldInfo(world,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X World Read Header test:" << endl;
    FileFormats::ReadExtendedWldFileHeader("test.wldx", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << world.meta.ERROR_info;
    } else {
        printWorldInfo(world,cout);
    }

    printLine(cout);
    cout << "\n\nPGE-X World Read test:" << endl;
    FileFormats::OpenWorldFile("test.wldx", world);
    cout << world.meta.filename << "\n";
    cout << world.meta.path << "\n";
    if(!world.meta.ReadFileValid)
    {
        cout << "Invalid file\n" << world.meta.ERROR_info;
    } else {
        printWorldInfo(world,cout);
    }


    PGE_FileFormats_misc::FileInfo x("melw.shit.LvlVX");
    cout << "\n\n\n";
    cout << "Name: " << x.filename() << endl;
    cout << "FPat: " << x.fullPath() << endl;
    cout << "BNam: " << x.basename() << endl;
    cout << "Sufx: " << x.suffix() << endl;
    cout << "Dirt: " << x.dirpath() << endl;




    //Deep tests of the level file formats
    #define ENABLE_SMBX64_DEEPTEST
    #define ENABLE_SMBX38A_DEEPTEST
    #define ENABLE_PGEX_DEEPTEST //required SMBX64 deeptest to pre-generate LVLX files!

    #ifdef ENABLE_SMBX64_DEEPTEST
    /**********************DEEP TEST OF SMBX64 files*********************/
    {
        //#define GENERATE_LVLX_FILES
        cout << "==================DEEP TEST OF SMBX64==================\n";
        QString path = "../PGEFileLib_test_files/smbx64/";
        QString wpath = "../PGEFileLib_test_files/smbx64_out/";
        #ifdef GENERATE_LVLX_FILES
        QString xpath = "../PGEFileLib_test_files/pgex/";
        #endif
        QDir testDir(path);
        testDir.mkdir("../smbx64_out/");
        #ifdef GENERATE_LVLX_FILES
        testDir.mkdir("../pgex/");
        #endif
        QStringList files = testDir.entryList(QDir::NoDotAndDotDot|QDir::Files);

        QFile newInvalid("invalid_s64.log");
        newInvalid.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream niout(&newInvalid);

        QFile times("times_s64.log");
        times.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream timesout(&times);

        QElapsedTimer meter;
        meter.start();

        foreach(QString file, files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path+file, false);
            LevelData FileDataNew;

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX64LvlFile(fileI, FileDataNew))
            {
                qint64 got = meter.elapsed();
                timesout << flString(file, 30) << " READ -> " << flString(QString::number(got), 20);

                meter.restart();
                FileFormats::smbx64LevelPrepare(FileDataNew);
                FileFormats::WriteSMBX64LvlFileF(wpath+file, FileDataNew, FileDataNew.meta.RecentFormatVersion);
                got = meter.elapsed();
                timesout << " WRITE -> " << got << "\r\n";
                timesout.flush();
                #ifdef GENERATE_LVLX_FILES
                FileFormats::WriteExtendedLvlFileF(xpath+file+"x", FileDataNew);
                #endif
            } else {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileDataNew.meta.ERROR_info;
                niout << path+file << "\r\nInfo: "
                      << FileDataNew.meta.ERROR_info << "\r\nlinedata" << FileDataNew.meta.ERROR_linedata
                      << "\r\nline:" << FileDataNew.meta.ERROR_linenum << "\r\n\r\n";
                newInvalid.flush();
            }
            cout.flush();
        }
        newInvalid.close();
        cout << "==================DEEP TEST OF SMBX64=END==============\n";
        cout.flush();
    }
    /*********************************************************************/
    #endif //ENABLE_SMBX64_DEEPTEST

    #ifdef ENABLE_SMBX38A_DEEPTEST
    /**********************DEEP TEST OF SMBX38A files*********************/
    {
        cout << "==================DEEP TEST OF SMBX38A==================\n";
        QString path = "../PGEFileLib_test_files/smbx38a/";
        QString opath = "../PGEFileLib_test_files/smbx38a_lvlx_diffs/";
        QString wpath = "../PGEFileLib_test_files/smbx38a_out/";
        QDir testDir(path);
        testDir.mkdir("../smbx38a_lvlx_diffs");
        testDir.mkdir("../smbx38a_out");
        QStringList files = testDir.entryList(QDir::NoDotAndDotDot|QDir::Files);

        QFile newInvalid("invalid_new.log");
        newInvalid.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream niout(&newInvalid);

//        QFile oldInvalid("invalid_old.log");
//        oldInvalid.open(QIODevice::WriteOnly|QIODevice::Truncate);
//        QTextStream oiout(&oldInvalid);

        QFile diffs("differents.log");
        diffs.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream diout(&diffs);

        QFile times("times.log");
        times.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream timesout(&times);

        QElapsedTimer meter;
        meter.start();

        foreach(QString file, files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path+file, false);
            QString raw_old;
            QString raw_new;
            LevelData FileDataNew;
            LevelData FileDataOld;

            qint64 time_old=0;
            qint64 time_new=0;

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX38ALvlFile(fileI, FileDataNew))
            {
                qint64 got = meter.elapsed();
                time_new=got;
                timesout << flString(file, 30) << " NEW -> " <<  flString(QString::number(got), 20);
                FileFormats::smbx64CountStars( FileDataNew );
                meter.restart();
                FileFormats::WriteExtendedLvlFileRaw(FileDataNew, raw_new);
                got = meter.elapsed();
                timesout << " WRITE -> " << flString(QString::number(got), 20);

                FileFormats::WriteSMBX38ALvlFileF(wpath+file, FileDataNew);
            } else {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileDataNew.meta.ERROR_info;
                niout << path+file << "\r\nInfo: "
                      << FileDataNew.meta.ERROR_info << "\r\nlinedata" << FileDataNew.meta.ERROR_linedata
                      << "\r\nline:" << FileDataNew.meta.ERROR_linenum << "\r\n\r\n";
                newInvalid.flush();
            }

#if 0
            FileDataOld = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadSMBX38ALvlFile_OLD(fileI, FileDataOld))
            {
                qint64 got = meter.elapsed();
                time_old=got;
                timesout << " OLD -> " << flString(QString::number(got), 20);
                if(time_old>time_new)
                    timesout << " NEW READS FASTER";
                else if(time_old<time_new)
                    timesout << " OLD READS FASTER";
                else
                    timesout << " BOTH ARE SAME";
                timesout << "\n";
                FileFormats::smbx64CountStars( FileDataOld );
                FileFormats::WriteExtendedLvlFileRaw(FileDataOld, raw_old);
            } else {
                cout << "OLD PARSER FAILED: Invalid file\n" << FileDataOld.meta.ERROR_info;
                oiout << path+file << "\r\nInfo: "
                      << FileDataOld.meta.ERROR_info << "\r\nlinedata" << FileDataOld.meta.ERROR_linedata
                      << "\r\nline:" << FileDataOld.meta.ERROR_linenum << "\r\n\r\n";
                oldInvalid.flush();
            }

            if(raw_old.isEmpty())
                continue;
            if(raw_new.isEmpty())
                continue;
            if(raw_old!=raw_new)
            {
                cout << "FILES ARE DIFFERENT\n";
                diout << path+file << "\r\n";
                diffs.flush();
                FileFormats::WriteExtendedLvlFileF(opath+file+".old.lvlx", FileDataNew);
                FileFormats::WriteExtendedLvlFileF(opath+file+".new.lvlx", FileDataOld);
            } else {
                //Remove similar files!
                QFile(opath+file+".old.lvlx").remove();
                QFile(opath+file+".new.lvlx").remove();
            }
#endif
            QFile(opath+file+".old.lvlx").remove();
            QFile(opath+file+".new.lvlx").remove();
            cout.flush();
        }

//        oldInvalid.close();
        newInvalid.close();
        diffs.close();
        cout << "==================DEEP TEST OF SMBX38A=END==============\n";
        cout.flush();
    }
    /*********************************************************************/
    #endif//ENABLE_SMBX38A_DEEPTEST


    #if defined(ENABLE_SMBX64_DEEPTEST) && defined(ENABLE_PGEX_DEEPTEST)
    /**********************DEEP TEST OF PGE-X files*********************/
    {
        cout << "==================DEEP TEST OF PGE-X==================\n";
        QString path = "../PGEFileLib_test_files/pgex/";
        QString wpath = "../PGEFileLib_test_files/pgex_out/";
        QDir testDir(path);
        testDir.mkdir("../pgex_out");
        QStringList files = testDir.entryList(QDir::NoDotAndDotDot|QDir::Files);

        QFile newInvalid("invalid_pgex.log");
        newInvalid.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream niout(&newInvalid);

        QFile times("times_pgex.log");
        times.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QTextStream timesout(&times);

        QElapsedTimer meter;
        meter.start();

        foreach(QString file, files)
        {
            PGE_FileFormats_misc::TextFileInput fileI(path+file, false);
            LevelData FileDataNew;

            FileDataNew = FileFormats::CreateLevelData();
            fileI.seek(0, PGE_FileFormats_misc::TextInput::begin);

            meter.restart();
            if(FileFormats::ReadExtendedLvlFile(fileI, FileDataNew))
            {
                qint64 got = meter.elapsed();
                timesout << flString(file, 30) << " READ -> " << flString(QString::number(got), 20);

                meter.restart();
                FileFormats::smbx64CountStars( FileDataNew );
                FileFormats::WriteExtendedLvlFileF(wpath+file, FileDataNew);
                got = meter.elapsed();
                timesout << " WRITE -> " << got << "\r\n";
                timesout.flush();

            } else {
                cout << "NEW PARSER FAILED: Invalid file\n" << FileDataNew.meta.ERROR_info;
                niout << path+file << "\r\nInfo: "
                      << FileDataNew.meta.ERROR_info << "\r\nlinedata" << FileDataNew.meta.ERROR_linedata
                      << "\r\nline:" << FileDataNew.meta.ERROR_linenum << "\r\n\r\n";
                newInvalid.flush();
            }
            cout.flush();
        }
        newInvalid.close();
        cout << "==================DEEP TEST OF PGE-X=END==============\n";
        cout.flush();
    }
    /*********************************************************************/
    #endif

    cout << "!!!!!!!!!!!!!!!!!!!!!EVERYTHING HAS BEEN DONE!!!!!!!!!!!!!!!!!!!!!\n";
    cout.flush();


    a.exit(0);
    return 0;
}
