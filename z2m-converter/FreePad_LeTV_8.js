module.paths.push(require.main.path+ '/node_modules') //sovle Cannot find module 'zigbee-herdsman-converters'

const {
    fromZigbeeConverters,
    toZigbeeConverters,
    exposes
} = require('zigbee-herdsman-converters');

const e = exposes.presets;

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

const ACCESS_STATE = 0b001, ACCESS_WRITE = 0b010, ACCESS_READ = 0b100;

const device = {
    zigbeeModel: ['FreePad_LeTV_8'],
    model: 'FreePad_LeTV_8',
    vendor: 'DIYRuZ',
    description: '[LeTV 8 button keypad](https://modkam.ru/?p=1791)',
    supports: 'single, double, triple, quadruple, many, hold/release',
    fromZigbee: [fromZigbeeConverters.diyruz_freepad_clicks, fromZigbeeConverters.diyruz_freepad_config, fromZigbeeConverters.battery],
    exposes: [e.battery(), e.action(['*_single', '*_double', '*_triple', '*_quadruple', '*_release'])].concat(((enpoinsCount) => {
        const features = [];
        for (let i = 1; i <= enpoinsCount; i++) {
            const epName = `button_${i}`;
            features.push(
                exposes.enum('switch_type', exposes.access.ALL, ['toggle', 'momentary', 'multifunction']).withEndpoint(epName));
            features.push(exposes.enum('switch_actions', exposes.access.ALL, ['on', 'off', 'toggle']).withEndpoint(epName));
        }
        return features;
    })(8)),
    toZigbee: [toZigbeeConverters.diyruz_freepad_on_off_config, toZigbeeConverters.factory_reset],
    meta: {configureKey: 1},
    configure: async (device, coordinatorEndpoint) => {
        const endpoint = device.getEndpoint(1);
        await bind(endpoint, coordinatorEndpoint, ['genPowerCfg']);
        if (device.applicationVersion < 3) { // Legacy PM2 firmwares
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
            if (ep.outputClusters.includes(18)) {
                await bind(ep, coordinatorEndpoint, ['genMultistateInput']);
            }
        });
    },
    endpoint: (device) => {
        return {
            button_1: 1, button_2: 2, button_3: 3, button_4: 4, button_5: 5,
            button_6: 6, button_7: 7, button_8: 8
        };
    },
};

module.exports = device;
