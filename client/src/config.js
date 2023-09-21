const environment = import.meta.env.PROD;

const production = {
    apiUrl: "/",
    socketUrl: "/",
    debug: false,
};
const development = {
    apiUrl: "http://localhost:5000/",
    socketUrl: "http://localhost:5000/",
    debug: true,
};

export default environment ? production : development;
