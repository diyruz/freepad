module.paths.push(require.main.path+ '/node_modules') //sovle Cannot find module 'zigbee-herdsman-converters'

const zigbeeHerdsmanConverters = require('zigbee-herdsman-converters');
const genMultistateInputClusterId = 18;

const getKey = (object, value) => {
    for (const key in object) {
        if (object[key] == value) return key;
    }
};
const bind = async (endpoint, target, clusters) => {
    for (const cluster of clusters) {
        await endpoint.bind(cluster, target);
    }
};

const device = {
    zigbeeModel: ['FreePad_LeTV_8'],
    model: 'FreePad_LeTV_8',
    vendor: 'DIYRuZ',
    description: '[LeTV 8 button keypad](https://modkam.ru/?p=1791)',
    supports: 'single, double, triple, quadruple, many, hold/release',
    fromZigbee: [zigbeeHerdsmanConverters.fromZigbeeConverters.diyruz_freepad_clicks, zigbeeHerdsmanConverters.fromZigbeeConverters.battery],
    toZigbee: [zigbeeHerdsmanConverters.toZigbeeConverters.diyruz_freepad_on_off_config, zigbeeHerdsmanConverters.toZigbeeConverters.factory_reset],
    meta: {
        configureKey: 1,
    },
    configure: async (device, coordinatorEndpoint) => {
        const endpoint = device.getEndpoint(1);
        await bind(endpoint, coordinatorEndpoint, ['genPowerCfg']);
        if (device.applicationVersion < 3) { //Legacy PM2 firmwares
            const payload = [{
                attribute: 'batteryPercentageRemaining',
                minimumReportInterval: 0,
                maximumReportInterval: 3600,
                reportableChange: 0,
            }, {
                attribute: 'batteryVoltage',
                minimumReportInterval: 0,
                maximumReportInterval: 3600,
                reportableChange: 0,
            }];
            await endpoint.configureReporting('genPowerCfg', payload);
        }


        device.endpoints.forEach(async (ep) => {
            if (ep.outputClusters.includes(genMultistateInputClusterId)) {
                await bind(ep, coordinatorEndpoint, ['genMultistateInput']);
            }
        });
    },
    endpoint: (device) => {
        return {
            button_1: 1,
            button_2: 2,
            button_3: 3,
            button_4: 4,
            button_5: 5,
            button_6: 6,
            button_7: 7,
            button_8: 8
        };
    },
};

module.exports = device;
