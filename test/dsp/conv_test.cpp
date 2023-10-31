/**
 * @file conv_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for Convolution operations
 * @version 0.1
 * @date 2023-04-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "dsp/conv.hpp"

// Sample data to test convolution

std::vector<long double> input = {1, -2, 4.3, 5.62, -6.74, 0};
std::vector<long double> kernel = {0.5, 2, 0.3};
std::vector<long double> output = {0.5, 1, -1.5500000000000003, 10.81, 9.16, -11.794, -2.022, 0 };

// Sample data to test fast convolution:

std::vector<long double> finput = {99.17677068401510060207567501043043933340963941078103,-73.17381018978315315491001243691417717624512591845925,-77.22765823189884621784715878456035070718068538443302,57.53871502793175695597472889648121525768364153364166,85.14525219357003221732090587680023156856910017552830,16.42485109956405706278517806988853071127256739375299,-100.87586566704521220209788569938512213875103162172940,-3.52662845112857886915800377506756118860588016145017,-21.41198579763478740615569940428248873310710822382576,19.28127916733965811262490397978888425282752325804191,40.23509250999683465651051309663386148528977061603834,-28.59513215602710001567225934568629472287879947208896,-17.42826562389926025336450123299943321010547881954851,38.62532354658486203985661472364693784050779748192163,40.55640513995727920193644696136459145255301591856960,13.23100575421856150069930708349645466182106202241617,34.33126111817919902848580902969089933900588711598855,45.63334254717888510169147240101484679453814742641633,-111.69913739905820594213340471471954495126591602178207,-107.43547583143785113519631270085734300320821013261788,-61.09098657296430988937759119254106339702796192194217,21.21168600879040878281930001789639395903918410559117,92.85350463558468017285066003903891176521015740549117,-27.08225204275675294970272597845239130190592593620340,36.92949379818533517881539465107818093916571139709201,89.39186029653255181308329331615474517260474728296063,-111.23897312968386625941519015617384798038303298876926,-97.12954784361751959847126762330021591644253301617187,41.30487095208275850126735856961088390639576280707344,37.18691104330079012599225876023126085202554924762610,83.05880176838535264267614731233719521844254310541474,111.18664372524350049490207720824316573850705454980909,-65.54940876469438141398488435426018178799916564590909,-41.29868633828421930237891341721875181936541621936300,-96.16086942616991592916712103571838704803623348735092,-39.29485899930622722517896715368948593535690197045091,-42.79655910706297711758737640320752297484969297326441,-67.18700108260067340079135871095319682602447622118634,-70.68861821501090160105770698769198652605884846064477,68.11604301146178298644262615795474030478727508079763,97.17957967090929548386799179368522853539456669430298,26.57444169998011090609847234387738073336160791761266,108.34674864524740476685879361048825430437856363886501,83.92240132652421111531165623946204023328858867749689,-44.89319204416645415439453568348964141242957981373565,-100.01311201924417663389124194212883947241806577418272,-101.25104156325214728067731138519003900806763706501020,108.64720664058689848524368156266542077835244340603246,-90.52229001042182061458189494979390042837039421252568,13.70644341282025561138757668603249256174707668749692,8.06958635998360755522765949397103075422117379212091,15.93045157874676618009957795461617054084324883871276,65.37230302737045501068017687840102617659652769494984,-24.11692058819114242470248421493577985647406290137482,-21.54784863514467367844151505292202582455955514141552,-52.45392243198654285555113323049168352785927628300817,-33.31646436876434504290510208141714265067683254483205,-27.52626841731572346572438531050780663246866408112959,17.94375214658925703732550060740951415599444577367381,50.75270358505764421945085207550397023838145592893410,-86.91056395580469175542821058216831105506363408643911,63.30764940875225922418592443423910443091061355804563,-49.35221594142990366224268667799556465949307677822732,-26.95714053518990243260697312965280552235726726602889};
std::vector<long double> fkernel = {-11.80734188903005356119352148649709734700547713225258,-43.29319340022382020701291536851460576047044958197358,-64.17630845780394964015619643914627703803151695122101,104.50989799709621114623624325297545860051591183174358,-67.54924675624434138037515882962703814054560262028839,-35.62256701890017015408610312853318678082516500292344,-90.56879652452310644448929859857525727081461397668915,98.25142394972260913200909945286494997039742577650684,14.36314503452964214099371769167755489874416850083586,16.04989989745124049165557905452647906112811930487333,23.75179245744340824337066219806513012190341055032565,-36.54922893231345247456462562164496452714797501691879,-41.73609135034018349560804694997033416192134584327371,-101.06004217476679723981753363362127091184255105349756,-22.96647571779903411431687679364707499402899445470702,-83.36436283762429771734359384628910503180394246986107,-23.18358854998787260657547828723988645376451563328074,-64.40603271558259628404716502583426443547454040181868,-28.24664623959982236750613428002146192901595537898968,82.04512322302178903662177201083180500684153921627929,-104.19091796220226941260362819462622668000451240556420,46.50703472541259174618694889301791963589235728591599,-46.39953399264152354187927818676056036156905624473562,-19.17944557268083364992236681897325820325325389394493,55.71155272666841141423234730505506665223083870277145,-15.67342494114519107279515664163557273117959590596846,-109.54752387981775431066872495333666433288790872855367,41.25927862447510887461184324853863164819213303933425,23.89797440261951178764190608802127291017785380963092,43.34788750082323611173087522580111457846367816770413,-9.85147575152705250257074471468586051630520198358150,-38.70050806580580014584793342593110856713972675033896};
std::vector<long double> foutput = {-1171.01403891609929997, -3429.69091979895283262, -2285.01774685549489119, 17725.0318651298641779, -12886.891398832692035, -14233.9161108097798998, -129.962815192139284792, 27489.1872207860367059, 2024.19880452851794439, -26017.6149914563025405, 3938.46971911387903442, 54.3998191223645982395, 12046.0072115991553154, -4969.99815550088726024, 1241.46039291168120799, -15139.6833383969424114, -6252.41471213001358809, 590.60140923307605143, -6816.13996329207795632, 17400.5224289633263517, -3454.43162805126405579, 2481.76979631610181354, -2582.27724181681853377, 4902.1301612419981546, 30128.026768953785453, -6038.67059688085429725, -32819.0401951843669011, -7333.1095812745978324, 19275.6608958438737211, -15530.7372383791797956, -41101.1346118376230194, 15712.8641125292120524, 45075.1431625188617431, 13206.1291450218101575, 2644.49569714610407134, -18278.077114120386403, -3711.02860338548167984, -9839.98621422812299109, 36384.1901905615541182, 7052.64255064814807472, 8751.24486662635201384, -5239.89046841215757411, -18572.5657832294615108, -3658.73819367004589109, -14146.2227919825936278, -17356.9458070682192883, 12208.0053866019470092, -2470.87249229385383176, -23981.7619797898402236, 47772.2664911469947597, 59010.9650702113216099, -14277.4171759387803711, 27585.2744249571234949, -31016.7757403050077176, 4668.30052152558361955, -28304.2095112958552523, -33274.2565632166092016, -3698.16618313147851715, 17785.3039880342375945, 22838.945503188858801, 3272.47535003962533251, 23422.4146200948577494, -8560.15563919260101855, -9173.48819210593480733, 6003.3415135859023728, -28745.1391272377205794, 24465.6119435896704548, -23914.9594692074799518, 35283.7382069120578088, -9682.2229064843226638, 8791.55397234392921391, 30866.3942040671511666, -6776.26950168149241049, -11453.3583250954303701, -4735.41772742965869991, 8398.70999624310715692, 7543.34233567129754139, 8538.78935660660230766, -4517.35617340813470744, -799.19865890145886167, 21409.6877338455043969, -10990.7768447550291127, 9029.53541309423957273, -1049.94216859074334947, -10841.1287705306785378, 1515.65463577043051924, 9983.96728823332284719, -7698.12028868932375669, 9452.49590556520047002, -2532.03494905388506053, -655.355402122046435509, -43.7261506843722963511, -3132.38113320279285112, 2175.52344741962787666, 1043.25503471317733073};

TEST_CASE("Convolution Test", "[conv][dsp]") {

    SECTION("Length", "Ensures output size calculations are accurate") {

        // Size with smallest first:

        REQUIRE(length_conv(3, 4) == 6);

        // Size with largest first:

        REQUIRE(length_conv(8, 2) == 9);
    }

    // Create buffers:

    BufferPointer binput = std::make_unique<AudioBuffer>(input);
    BufferPointer bkernel = std::make_unique<AudioBuffer>(kernel);

    // Create buffer of results:

    AudioBuffer buff(length_conv(input.size(), kernel.size()));

    SECTION("Input Side", "Tests for the input side algorithm") {

        SECTION("Size", "Ensures the algorithm works with defined size") {

            // Do operation:

            input_conv(input.begin(), input.size(), kernel.begin(),
                       kernel.size(), buff.ibegin());

            // Check that output matches expected:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(output.at(iter.get_index()), 1e-10));
            }
        }

        SECTION("Iterator", "Ensures the algorithm works with iterators") {

            // Do operation:

            input_conv(input.begin(), input.end(), kernel.begin(), kernel.end(),
                       buff.ibegin());

            // Check that output matches expected:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(output.at(iter.get_index()), 1e-10));
            }
        }

        SECTION("Buffer", "Ensures the algorithm works with buffer pointers") {

            // Do operation:

            auto buff = input_conv(std::move(binput), std::move(bkernel));

            // Check that output matches expected:

            for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(output.at(iter.get_index()), 1e-10));
            }
        }
    }

    SECTION("Output Side", "Tests for the output side algorithm") {

        SECTION("Size", "Ensures the algorithm works with defined size") {

            // Do operation:

            output_conv(input.begin(), input.size(), kernel.begin(),
                        kernel.size(), buff.ibegin());

            // Check that output matches expected:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(output.at(iter.get_index()), 1e-10));
            }
        }

        SECTION("Iterator", "Ensures the algorithm works with iterators") {

            // Do operation:

            output_conv(input.begin(), input.end(), kernel.begin(), kernel.end(),
                        buff.ibegin());

            // Check that output matches expected:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(output.at(iter.get_index()), 1e-10));
            }
        }

        SECTION("Buffer", "Ensures the algorithm works with buffer pointers") {

            // Do operation:

            auto buff = input_conv(std::move(binput), std::move(bkernel));

            // Check that output matches expected:

            for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(output.at(iter.get_index()), 1e-10));
            }
        }
    }

    SECTION("Fast", "Tests for fast convolution, implemented via frequency domain multiplication") {

        int size = length_conv(finput.size(), fkernel.size());

        // Create buffer of results:

        std::vector<long double> buff(size);

        // Do operation:

        //conv_fft(finput.begin(), finput.size(), fkernel.begin(), fkernel.size(), buff.begin());

        // Check that output matches expected:

        for (int i = 0; i < size; ++i) {

            //REQUIRE_THAT(buff.at(i), Catch::Matchers::WithinAbs(foutput.at(i), 1e-10));
        }
    }
}